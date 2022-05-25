#include "ClientSocket.hpp"
#include "Response.hpp"

ClientSocket::ClientSocket(int fd, ConfigServer server_info)
	: Socket(CLIENT_SOCKET, fd), _server_info(server_info), _request(new Request(fd)), _response(NULL), _resource(new Resource()), _stage(GET_REQUEST)
{
	_response = new Response(_request, _resource);
};

ClientSocket::~ClientSocket(){

};

ConfigServer ClientSocket::getConnectServerInfo() { return _server_info; }

Request *ClientSocket::getRequest() const { return _request; }
Resource *ClientSocket::getResource() const { return _resource; }
Response *ClientSocket::getResponse() const { return _response; }

void ClientSocket::recieveRequest()
{
	_request->parseRequest();
	if (_request->getRequestStage() == READ_END_OF_REQUEST)
	{
		setStage(END_OF_REQUEST);
	}
}

Stage ClientSocket::getStage()
{
	return _stage;
}
void ClientSocket::setStage(Stage stage){ _stage = stage; }

void ClientSocket::sendResponse()
{
	_response->setEntity(_resource->getContent());
	return _response->combineResponse();
}

std::string ClientSocket::getErrorPage(std::string error_num)
{
	std::string root = "./static_file/defaultErrorPage.html";
	std::map<std::string, std::string> temp = _request->getRoute()->getCommonDirective()._error_page;
	std::string _status_code;

	_status_code = error_num;
	if (temp.find(_status_code) != temp.end())
	{
		root = _request->getRoute()->getCommonDirective()._root + "/" + _request->getRoute()->getCommonDirective()._error_page[_status_code];
	}
	return root;
}

void ClientSocket::setResourceFd()
{
	_request->setRoute(_server_info.getLocations());
	_resource->setExtension(getExtension(_request->getFile()));
	_resource->setContentType(getContentType(_resource->getSrcContentType()));

	std::vector<std::string> allowed_method = _request->getRoute()->getCommonDirective()._limit_except;

	if (find(allowed_method.begin(), allowed_method.end(), _request->getMethod()) == allowed_method.end())
	{
		std::cout << "ERROR Response " << std::endl;
		setErrorResource("403");
		return;
	}
	if (isCGI(_request->getFile()))
	{
		CgiHandler cgi(_request->getRoute(), this);
		cgi.executeCgi(); // <- 여기서 resource read_fd, write_fd 설정
		setStage(CGI_WRITE);
		return;
	}
	if (_request->getMethod() == "GET")
	{
		std::cout << "GET" << std::endl;
		setGetFd();
	}
	else if (_request->getMethod() == "POST")
	{
		std::cout << "POST" << std::endl;
		setPostFd();
	}
}

bool ClientSocket::isCGI(const std::string &path)
{
	std::string type;
	type = getExtension(path);
	std::map<std::string, std::string> cgi_path = _request->getRoute()->getCommonDirective()._cgi_path;
	std::cout << "cgi_path = " << type << std::endl;
	if (cgi_path.find(type) != cgi_path.end())
	{
		return 1;
	}
	return 0;
}

void ClientSocket::setGetFd()
{
	std::string entity_file;
	std::string root = _request->getRoute()->getCommonDirective()._root;
	std::vector<std::string> index_page = _request->getRoute()->getCommonDirective()._index;
	if (_request->getFile() == "") // 파일명이 없을때
	{
		for (size_t i = 0; i < index_page.size(); i++)
		{
			std::string indexfile = root + "/" + index_page[i];
			
			std::cout << i  << " " << indexfile << std::endl;
			if (isFile(indexfile) == 1)
			{
				_resource->setReadFd(open(indexfile.c_str(), O_RDONLY));
				_response->setStatusCode("200");
				return;
			}
		}
	}
	else
	{
		entity_file = root + _request->getFile();
		if (isFile(entity_file) == 1)
		{
			_resource->setReadFd(open(entity_file.c_str(), O_RDONLY));
			_response->setStatusCode("200");
			return;
		}
	}
	if (_request->getRoute()->getCommonDirective()._autoindex)
	{
		setStage(MAKE_AUTOINDEX);
		_response->setStatusCode("200");
		if (getFileType(entity_file)) // 존재하는 파일 or 디렉토리
		{
			// resource content에 autoindex 만들기 (_request->getFile() 기준)
			_resource->makeAutoIndex(root, _request->getFile());
			return;
		}
		// resource content에 autoindex 만들기 (/) // root
		_resource->makeAutoIndex(root, "/");
		return;
	}
	else
	{
		setErrorResource("404");
		return;
	}
}

void ClientSocket::setPostFd()
{
	std::string entity_file;
	std::string root = _request->getRoute()->getCommonDirective()._root;
	std::vector<std::string> index_page = _request->getRoute()->getCommonDirective()._index;
	std::string path = _request->getRoute()->getCommonDirective()._root;
	std::string dir, dirpath, filename;
	size_t rpos = _request->getFile().rfind("/");

	if (rpos != std::string::npos)
		dir = _request->getFile().substr(0, rpos);
	dir == "" ? dirpath = path : dirpath = dir;
	if (_request->getFile() == "")
	{
		filename = dirpath + "/NewFile";
		std::string temp = filename;
		int i = 0;
		temp = filename;
		while (getFileType(temp))
		{
			temp = filename + '(' + std::to_string(i) + ')';
			i++;
		}
		filename = temp;
		_response->setStatusCode("201");
	}
	else
	{
		filename = dirpath + _request->getFile();
	}
	if (isDirectory(filename))
	{
		setErrorResource("400");
		return;
	}
	if (isFile(filename))
	{
		int fd = open(filename.c_str(), O_WRONLY | O_APPEND);

		if (fd > 0)
		{
			setErrorResource("403");
			return;
		}
		_response->setStatusCode("200");
		_resource->setWriteFd(fd);
	}

	else
	{
		int fd = open(filename.c_str(), O_WRONLY | O_CREAT, 0777);
		if (fd < 0)
		{
			setErrorResource("500");
			return;
		}
		else
			_response->setStatusCode("201");
		_resource->setWriteFd(fd);
	}
}

void ClientSocket::setErrorResource(std::string error)
{
	int fd = open(getErrorPage(error).c_str(), O_RDONLY);
	_resource->setReadFd(fd);
	if (fd < 0)
	{
		_resource->setContent("Error");
		_resource->setContentType("text/plain");
	}
}

void ClientSocket::parsingCGIResponse()
{
	std::string content = getResource()->getContent();
	int start, end;
	std::string tem;

	while (1)
	{
		tem = content.substr(0, content.find('\n'));
		start = 0;
		if ((end = tem.find(": ")) < 0)
			break;
		getResponse()->addHeader(tem.substr(start, end - start), tem.substr(end + 2, tem.find('\n') - end + 2));  // cgi 반환값의 header 파싱
		content = content.substr(tem.size() + 1, content.size());
	}
	getResource()->setContent(content);

}
