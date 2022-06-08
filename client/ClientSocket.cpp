#include "ClientSocket.hpp"
#include "Response.hpp"

ClientSocket::ClientSocket(int fd, ConfigServer server_info)
	: Socket(CLIENT_SOCKET, fd),  _request(new Request(fd)), _response(new Response(fd)), _resource(new Resource()), 
		_stage(GET_REQUEST), _server_info(server_info), _request_parse_error(false)
{
};

ClientSocket::~ClientSocket()
{
	delete _request;
	delete _response;
	delete _resource;
	delete _route;
	_request = NULL;
	_response = NULL;
	_resource = NULL;
	_route = NULL;
};

ConfigServer ClientSocket::getConnectServerInfo() { return _server_info; }

Request *ClientSocket::getRequest() const { return _request; }
Resource *ClientSocket::getResource() const { return _resource; }
Response *ClientSocket::getResponse() const { return _response; }
ConfigLocation *ClientSocket::getRoute() const { return _route; }
std::string ClientSocket::getFile() const { return _file; }

void ClientSocket::readRequest()
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
void ClientSocket::setStage(Stage stage) { _stage = stage; }

void ClientSocket::makeResponse()
{
	_response->setEntity(_resource->getResourceContent());
	return _response->makeResponse(_resource, _route);
}

void ClientSocket::sendResponse()
{
	FILE *socket_write = _response->getSocketWriteFD();
	std::string combine_res = _response->combineResponse();

	fputs(combine_res.c_str(), socket_write);
	fflush(socket_write);
	fwrite(_response->getEntity().c_str(), sizeof(char), _response->getEntity().length(), socket_write);
	fclose(socket_write);
}

std::string ClientSocket::getErrorPage(std::string error_num)
{
	std::string root = "./setting/defaultErrorPage.html";
	std::map<std::string, std::string> temp = _route->getCommonDirective()._error_page;
	std::string _status_code;

	_status_code = error_num;
	if (temp.find(_status_code) != temp.end())
	{
		root = _route->getCommonDirective()._root + "/" + _route->getCommonDirective()._error_page[_status_code];
		if (getFileType(root) == 0)
		{
			return "./setting/defaultErrorPage.html";
		}
	}
	return root;
}

int ClientSocket::isErrorRequest()
{
	std::vector<std::string> allowed_method = _route->getCommonDirective()._limit_except;

	if (_request_parse_error)
	{
		setErrorResource("400");
	}
	else if (find(allowed_method.begin(), allowed_method.end(), _request->getMethod()) == allowed_method.end())
	{
		setErrorResource("405");
	}
	else if (_request->getRequestHeaderSize() > static_cast<unsigned int>(_route->getCommonDirective()._request_limit_header_size) || 
	_request->getRequestBodySize() > static_cast<unsigned long>(_route->getCommonDirective()._client_limit_body_size)) // request body & header size 체크
	{
		setErrorResource("413");
	}
	else if (isDirectory(_route->getCommonDirective()._root) == 0)
	{
		setErrorResource("404");
	}
	else
	{
		return 0;
	}
	return 1;
}

void ClientSocket::setResourceFd()
{
	setRoute();
	_resource->setResourceType(getContentType(_file));
	if (isErrorRequest())
	{
		return ;
	}
	if (isCGI(_file))
	{
		CgiHandler cgi(_route, this);
		if (cgi.executeCgi() != 200) // <- 여기서 resource read_fd, write_fd 설정
		{
			setErrorResource("500");
			return ;
		}
		setStage(CGI_WRITE);
	}
	else if (_request->getMethod() == "GET")
	{
		setGetFd();
	}
	else if (_request->getMethod() == "POST")
	{
		setPostFd();
	}
	else if (_request->getMethod() == "DELETE")
	{
		setDeleteFd();
	}
}

bool ClientSocket::isCGI(const std::string &path)
{
	std::string type;
	type = getExtension(path);
	std::map<std::string, std::string> cgi_path = _route->getCommonDirective()._cgi_path;
	if (cgi_path.find(type) != cgi_path.end())
	{
		return 1;
	}
	return 0;
}

void ClientSocket::setGetFd()
{
	std::string entity_file;
	std::string root = _route->getCommonDirective()._root;
	std::vector<std::string> index_page = _route->getCommonDirective()._index;
	entity_file = root + _file;
	if (_route->getCommonDirective()._autoindex && isDirectory(entity_file)) // 오토인덱스 on & 존재하는 파일 or 디렉토리
	{
		// resource content에 autoindex 만들기 
		_resource->makeAutoIndex(root, _file, _request->getRequestHeader()["Host"]);
		setStage(SEND_RESPONSE); // auto index 바로 response 보내기
		_response->setStatusCode("200");
		return ;
	}
	else if (_file == "") // 파일명이 없을때
	{
		for (size_t i = 0; i < index_page.size(); i++)
		{
			std::string indexfile = root + "/" + index_page[i];
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
		if (getFileType(entity_file) > 0)
		{
			_response->setStatusCode("200");
			if (isDirectory(entity_file))
			{
				_resource->setReadFd(open("./setting/defaultDirectoryPage.html", O_RDONLY));
			}
			else
			{
				_resource->setReadFd(open(entity_file.c_str(), O_RDONLY));
			}
			return ;
		}
	}
	setErrorResource("404");
}

void ClientSocket::setPostFd()
{
	std::string root = _route->getCommonDirective()._root;
	std::string entire_file = root + _file;
	std::string dirpath, filepath, filename;
	int i = 1;
	dirpath = root;
	filepath = _file;

	if (isDirectory(entire_file))
	{
		dirpath = entire_file;
		filepath = "";
	}
	std::cout << entire_file << std::endl;
	if (filepath == "")
	{
		filename = dirpath + "/NewFile";
		std::string temp = filename;
		temp = filename;
		while (getFileType(temp))
		{
			temp = filename + '(' + std::to_string(i) + ')';
			i++;
		}
		filename = temp;
		_response->setStatusCode("201");
	}
	else if (getFileType(entire_file) == 0)
	{
		_response->setStatusCode("201");
		filename = entire_file;
	}
	else if (getFileType(entire_file) == 1)
	{
		_response->setStatusCode("200");
		filename = entire_file;
	}
	size_t rrpos = filename.rfind("/");
	std::string temp_file;
	if (rrpos == std::string::npos)
		std::string temp_file = filename;
	else
		temp_file = filename.substr(0, rrpos);
	if (isDirectory(filename) || !isDirectory(temp_file))
	{
		setErrorResource("400");
		return;
	}
	int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0777);
	if (fd < 0)
	{
		setErrorResource("500");
		return;
	}
	_resource->setWriteFd(fd);
}


void ClientSocket::setDeleteFd()
{
	std::string entity_file;
	std::string root = _route->getCommonDirective()._root;
	std::vector<std::string> index_page = _route->getCommonDirective()._index;
	entity_file = root + _file;
	if (_file == "") // 파일명이 없을때
	{
		for (size_t i = 0; i < index_page.size(); i++)
		{
			std::string indexfile = root + "/" + index_page[i];
			if (isFile(indexfile) == 1)
			{
				_response->setStatusCode("200");
				setStage(SEND_RESPONSE);
				return;
			}
		}
	}
	else
	{
		if (getFileType(entity_file) > 0)
		{
			setStage(SEND_RESPONSE);
			_response->setStatusCode("200");
			return ;
		}
	}
	setErrorResource("404");
}

void ClientSocket::setErrorResource(std::string error)
{
	int fd;
	_response->setStatusCode(error);
	fd = open(getErrorPage(error).c_str(), O_RDONLY);
	if (_resource->getReadFd() != -1)
	{
		close(_resource->getReadFd());
	}
	_resource->setReadFd(fd);
	if (fd < 0)
	{
		_resource->setResourceContent("Error");
		_resource->setResourceType("text/plain");
	}
}

void ClientSocket::parsingCGIResponse()
{
	std::string content = getResource()->getResourceContent();
	int start, end;
	std::string tem;

	while (1)
	{
		tem = content.substr(0, content.find('\n'));
		start = 0;
		if ((end = tem.find(": ")) < 0)
			break;
		getResponse()->addHeader(tem.substr(start, end - start), tem.substr(end + 2, tem.find('\n') - end + 2)); // cgi 반환값의 header 파싱
		content = content.substr(tem.size() + 1, content.size());
	}
	getResource()->setResourceContent(content);
}

void ClientSocket::setRoute()
{
	std::string path = _request->getPath();
	int path_len = path.size();
	std::vector<ConfigLocation> locations = _server_info.getLocations();
	_file = "";

	for (int i = path_len - 1; i >= -1; i--)
	{
		if (i == path_len - 1 || path[i + 1] == '/')
		{
			for (size_t j = 0; j < locations.size(); j++)
			{
				if (path.substr(0, i + 1) == locations[j].getUrl() || path.substr(0, i + 1) == locations[j].getUrl() + "/")
				{
					_route = new ConfigLocation(locations[j].getUrl(), locations[j].getCommonDirective(), locations[j].getReturnCode(),
					locations[j].getReturnData());
					if (i != path_len - 1)
						_file = path.substr(i + 1);
					return;
				}
				else if (i == -1 && locations[j].getUrl() == "/")
				{
					_route = new ConfigLocation("/", locations[j].getCommonDirective(), locations[j].getReturnCode(),
					locations[j].getReturnData());
					_file = path;
					return;
				}
			}
		}
	}
}

bool ClientSocket::getRequestParseError() const
{
	return _request_parse_error;
}

void ClientSocket::setRequestParseError(bool a)
{
	_request_parse_error = a;
}
