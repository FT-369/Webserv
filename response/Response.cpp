#include "Response.hpp"

Response::Response(Request *request)
	: _request(request), _socket_write(NULL), _route(NULL)
{
	_socket_write = fdopen(dup(_request->getSocketFD()), "w");
}

Response::~Response()
{
}

FILE* Response::getSocketWriteFD() { return _socket_write; }

std::string Response::getContentType(std::string file)
{
	// std::string content_type = GlobalConfig::getDefaultType();
	std::string content_type = "text/html";

	size_t rpos = file.rfind(".");
	std::string extension;

	if (rpos == std::string::npos)
	{
		return content_type;
	}
	extension = file.substr(rpos + 1);

	if (GlobalConfig::getMimeTypes().find(extension) != GlobalConfig::getMimeTypes().end())
		content_type = GlobalConfig::getMimeTypes()[extension];
	return content_type;
}

void Response::makeStartLine()
{
	_start_line = _request->_protocol + " " + _status + " " + GlobalConfig::getStatusCode()[_status] + "\r\n";
}

void Response::makeHeader()
{
	_header["Content-type"] = getContentType(_request->getPath());
	_header["Content-length"] = std::to_string(_entity.length());
	_header["Server"] = "Mac Web Server";
}

void Response::makeEntity(std::string file)
{
	std::string buffer;
	std::string filePath;
	std::ifstream is(file);

	if (is.fail())
	{
		makeErrorResponse("404");
		return; // throw error
	}
	while (std::getline(is, buffer))
	{
		_entity += buffer + "\n";
	}
	_status = "200";
}

void Response::setRedirect()
{

	if (_route->getReturnCode() == "")
		return;
	_status = _route->getReturnCode();
	_header["Location"] = _route->getReturnData();
}

void Response::mappingPath(std::vector<ConfigLocation> locations)
{
	// std::vector<ConfigLocation> locations = _server_info.getLocations();
	std::string path = _request->getPath();
	int path_len = path.size();
	std::cout << "path : " << path << std::endl;
	_file = "";
	for (int i = path_len - 1; i >= -1; i--)
	{
		if (i == path_len - 1 || path[i + 1] == '/')
		{
			for (int j = 0; j < locations.size(); j++)
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
					_file = _request->getPath();
					return;
				}
			}
		}
	}
}

void Response::makeResponse()
{
	std::vector<std::string> temp;

	//요청 url <=> location 매핑
	// mappingPath();
	temp = _route->getCommonDirective()._limit_except;
	std::cout << "[Mapping Path] url: " << _route->getUrl() << ", file:" << _file << std::endl;
	if (find(temp.begin(), temp.end(), _request->getMethod()) == temp.end())
	{
		std::cout << "ERROR Response " << std::endl;
		makeErrorResponse("405");
	}
	else if (_request->getMethod() == "GET")
	{
		std::cout << "GET" << std::endl;
		makeGetResponse();
	}
	else if (_request->getMethod() == "POST")
	{
		std::cout << "POST" << std::endl;
		// makePostResponse();
	}
	else if (_request->getMethod() == "DELETE")
	{
		std::cout << "DELETE" << std::endl;
		makeDeleteResponse();
	}
	makeHeader();
	makeStartLine();
	setRedirect();
	makeHeader();
	makeStartLine();
}

void Response::combineResponse()
{
	std::string send_data;
	std::map<std::string, std::string>::iterator it;

	makeResponse();

	send_data += _start_line;
	for (it = _header.begin(); it != _header.end(); it++)
	{
		send_data += it->first + ": " + it->second + "\r\n";
	}
	send_data += "\r\n" + _entity + "\r\n";

	fputs(send_data.c_str(), _socket_write);
	fflush(_socket_write);
	fclose(_socket_write);
}

std::string Response::settingRoute()
{
	std::string entityFile = "./static_file/404.html";
	std::string root = _route->getCommonDirective()._root;
	std::vector<std::string> indexPage = _route->getCommonDirective()._index;

	if (_file == "")
	{
		if (!indexPage.empty())
		{
			for (size_t i = 0; i < indexPage.size(); i++)
			{
				std::ifstream idx(root + "/" + indexPage[i]);
				if (idx.is_open())
				{
					entityFile = root + "/" + indexPage[i];
					break;
				}
			}
		}
		else if (_route->getCommonDirective()._autoindex)
		{
			entityFile = root + "/autoindex.html";
		}
	}
	else
	{
		std::ifstream is(root + _file);

		if (!is.fail())
		{
			entityFile = root + _file;
		}
		else if (_route->getCommonDirective()._autoindex)
		{
			entityFile = root + "/autoindex.html";
		}
	}
	return entityFile;
}

void Response::makeGetResponse()
{
	makeEntity(settingRoute());

}

void Response::makeDeleteResponse()
{
	makeEntity(settingRoute());
}

void Response::makeErrorResponse(std::string error_num)
{
	std::string root = "./static_file/defaultErrorPage.html";
	std::map<std::string, std::string> temp = _route->getCommonDirective()._error_page;
	std::string buffer;

	_status = error_num;
	if (temp.find(_status) != temp.end())
	{
		root =_route->getCommonDirective()._root + "/" + _route->getCommonDirective()._error_page[_status];
	}
	std::ifstream idx(root);
	if (idx.is_open() == false)
	{
		_entity = "";
		return ;
	}
	while (std::getline(idx, buffer))
	{
		_entity += buffer + "\n";
	}
}
