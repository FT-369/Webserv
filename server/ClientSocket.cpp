#include "ClientSocket.hpp"
#include "Response.hpp"

ClientSocket::ClientSocket(int fd, ConfigServer server_info)
	: Socket(CLIENT_SOCKET, fd), _server_info(server_info), _request(new Request(fd)), _response(NULL)
{
	_response = new Response(_request);
};

ClientSocket::~ClientSocket(){

};

ConfigServer ClientSocket::getConnectServerInfo() { return _server_info; }

Request *ClientSocket::getRequest() const { return _request; }
Resource *ClientSocket::getResource() const { return _resource; }
Response *ClientSocket::getResponse() const { return _response; }

int ClientSocket::recieveRequest()
{
	return _request->parseRequest();
}

RequestStage ClientSocket::getRequestStage()
{
	return _request->getStage();
}

void ClientSocket::sendResponse()
{
	// _request->setRoute(_server_info.getLocations());
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
		root =_request->getRoute()->getCommonDirective()._root + "/" + _request->getRoute()->getCommonDirective()._error_page[_status_code];
	}
	return root;
}

void ClientSocket::setResourceFd()
{
	_request->setRoute(_server_info.getLocations());

	std::string entity_file;
	std::string root = _request->getRoute()->getCommonDirective()._root;
	std::vector<std::string> index_page = _request->getRoute()->getCommonDirective()._index;
	std::vector<std::string> allowed_method = _request->getRoute()->getCommonDirective()._limit_except;

	if (find(allowed_method.begin(), allowed_method.end(), _request->getMethod()) == allowed_method.end())
	{
		std::cout << "ERROR Response " << std::endl;
		int fd = open(getErrorPage("405").c_str(), O_RDONLY);
		_resource->setReadFd(fd);
		if (fd < 0)
		{
			_resource->setContent("Error");
			// content type은 text plain
		}
		return ;
	}
	if (isCGI(_request->getFile()))
	{
		CgiHandler cgi(_request->getRoute(), _request);
		cgi.executeCgi(); // <- 여기서 resource read_fd, write_fd 설정
		return ;
	}
	if (_request->getMethod() == "GET")
	{
		if (_request->getFile() == "") // 파일명이 없을때
		{
			for (size_t i = 0; i < index_page.size(); i++)
			{
				std::string indexfile = root + "/" + index_page[i];
				if (isFile(indexfile) == 1)
				{
					_resource->setReadFd(open(indexfile.c_str(), O_RDONLY));
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
				return;
			}
		}
		if (_request->getRoute()->getCommonDirective()._autoindex)
		{
			if (getFileType(entity_file)) // 존재하는 파일 or 디렉토리
			{
				// resource content에 autoindex 만들기 (_request->getFile() 기준)
				return ;
			}
			// resource content에 autoindex 만들기 (/) // root
			return ;
		}
		else 
		{
			// _resource->setReadFd(open(getErrorPage("404"), O_RDONLY));
			return;
		}
	}
	else if (_request->getMethod() == "POST")
	{
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
		}
		else
		{
			filename = dirpath + _request->getFile();
		}
		if (isDirectory(filename))
		{
			_response->setStatusCode("400");
			return;
		}
		if (isFile(filename))
		{
			int fd = open(filename.c_str(), O_WRONLY | O_APPEND);
			
			if (fd > 0)
			{
				_response->setStatusCode("403");
				return;
			}
			_response->setStatusCode("200");
			_resource->setWriteFd(fd);
		}

		else
		{
			int fd = open(filename.c_str(), O_WRONLY | O_APPEND, 0777);
			if (fd < 0)
			{
				_response->setStatusCode("500");
				return;
			}
			else
				_response->setStatusCode("201");
				_resource->setWriteFd(fd);
		}
		//
	}
}

std::string ClientSocket::getCgiType(std::string file)
{
	std::string cgi_type = "php";

	size_t rpos = file.rfind(".");
	std::string extension = "";

	if (rpos != std::string::npos)
	{
		extension = file.substr(rpos + 1);
	}
	return extension;
}

bool ClientSocket::isCGI(const std::string &path)
{
	std::string type;
	type = getCgiType(path);
	std::map<std::string, std::string> cgi_path = _request->getRoute()->getCommonDirective()._cgi_path;
	if (cgi_path.find(type) != cgi_path.end())
	{
		return 1;
	}
	return 0;
}
