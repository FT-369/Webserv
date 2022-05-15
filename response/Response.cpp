#include "Response.hpp"

Response::Response(std::map<std::string, std::string> const &mime_types, std::vector<ConfigLocation> locations)
	: _mime_types(mime_types), _request(NULL), _locations(locations), _route(NULL)
{
}

Response::Response(std::map<std::string, std::string> const &mime_types, Request *request, std::vector<ConfigLocation> locations)
	: _mime_types(mime_types), _request(request), _locations(locations), _route(NULL)
{
}

Response::~Response()
{
}

std::map<std::string, std::string> Response::getMimeType() { return _mime_types; }

std::string Response::getContentType(std::string file)
{
	// std::string content_type = config.getDefaultType();
	std::string content_type = "text/html";

	size_t rpos = file.rfind(".");
	std::string extension;

	if (rpos == std::string::npos)
	{
		return content_type;
	}
	extension = file.substr(rpos + 1);

	if (_mime_types.find(extension) != _mime_types.end())
		content_type = _mime_types[extension];
	return content_type;
}

void Response::makeStartLine()
{
	_start_line = _request->_protocol + " " + _status + " " + _status_code[_status] + "\r\n";
}

void Response::makePostHeader()
{
	_header["Server"] = "Mac Web Server";
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

void Response::mappingPath()
{
	std::string path = _request->getPath();
	int path_len = path.size();
	std::cout << "path : " << path << std::endl;
	_file = "";
	for (int i = path_len - 1; i >= -1; i--)
	{
		if (i == path_len - 1 || path[i + 1] == '/')
		{
			for (int j = 0; j < _locations.size(); j++)
			{
				if (path.substr(0, i + 1) == _locations[j].getUrl() || path.substr(0, i + 1) == _locations[j].getUrl() + "/")
				{
					_route = new ConfigLocation(_locations[j].getUrl(), _locations[j].getCommonDirective(), _locations[j].getReturnCode(),
					_locations[j].getReturnData());
					if (i != path_len - 1)
						_file = path.substr(i + 1);
					return;
				}
				else if (i == -1 && _locations[j].getUrl() == "/")
				{
					_route = new ConfigLocation("/", _locations[j].getCommonDirective(), _locations[j].getReturnCode(),
					_locations[j].getReturnData());
					_file = _request->getPath();
					return;
				}
			}
		}
	}
}

std::string Response::makeResponse()
{
	std::string send_data;
	std::map<std::string, std::string>::iterator it;
	std::vector<std::string> temp;
	//요청 url <=> location 매핑
	mappingPath();
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
	send_data += _start_line;
	for (it = _header.begin(); it != _header.end(); it++)
	{
		send_data += it->first + ": " + it->second + "\r\n";
	}
	send_data += "\r\n" + _entity + "\r\n";
	return send_data;
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


int	Response::isDirectory(const std::string &path)
{
	struct stat sb;

	if (stat(path.c_str(), &sb) != 0)
		return 0;
	if ((sb.st_mode & S_IFMT) & S_IFDIR)
		return 1;
	else 
		return 0;

}

int	Response::isFile(const std::string &path)
{
	struct stat sb;
	
	if (stat(path.c_str(), &sb) != 0)
		return 0;
	if ((sb.st_mode & S_IFMT) & S_IFREG)
		return 1;
	else
		return 0;
}

void	Response::makePostResponse()
{
	std::string path = _route->getCommonDirective()._root;
	std::string dir, filename;
	size_t rpos = _file.rfind("/");
	std::string dirpath;

	if (rpos != std::string::npos)
		dir = _file.substr(0, rpos);
	if (dir == "")
	{
		dirpath = path;
		if (_file == "")
			filename = dirpath + "/NewFile";
		else
			filename = dirpath + _file;
	}
	else
	{
		dirpath = dir;
		if (_file == "")
			filename = dirpath + "/NewFile";
		else
			filename = _file;
	}
	// creatfilename();
	if (isDirectory(filename))
	{
		_status = "400";
		//makeErrorPage()
		return;
		//throw 400
	}
	if (isFile(filename))
	{
		std::ofstream ofs;

		ofs.open(filename, std::ios::app);
		if (ofs.is_open())
			_status = "200";
		else
		{
			_status = "403";
			// makeErrorPage();
			return ;
		}
		ofs << _request->_request_body;
		ofs.close();
	}
	else
	{
		std::ofstream ofs(filename);
		if (ofs.fail())
		{
			_status = "500";
			// makeErrorPage();
			return ;
		}
		else
			_status = "201";
		ofs << _request->_request_body;
		ofs.close();
	}
	// makeEntity(settingRoute()); entity가 없다 post는
	makePostHeader();
	makeStartLine();
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

void Response::setStatusCode()
{
	_status_code["100"] = "Continue";
	_status_code["101"] = "Switching Protocols";
	_status_code["200"] = "OK";
	_status_code["201"] = "Created";
	_status_code["202"] = "Accepted";
	_status_code["203"] = "Non-Authoritative";
	_status_code["204"] = "No Content";
	_status_code["205"] = "Reset Content";
	_status_code["206"] = "Partial Content";
	_status_code["300"] = "Multiple Choices";
	_status_code["301"] = "Moved Permanently";
	_status_code["302"] = "Found";
	_status_code["303"] = "See Other";
	_status_code["304"] = "Not Modified";
	_status_code["305"] = "Use Proxy";
	_status_code["307"] = "Temporary Redirect";
	_status_code["400"] = "Bad Request";
	_status_code["401"] = "Unauthorized";
	_status_code["403"] = "Forbidden";
	_status_code["404"] = "Not Found";
	_status_code["405"] = "Method Not Allowed";
	_status_code["406"] = "Not Acceptable";
	_status_code["407"] = "Proxy Authentication";
	_status_code["408"] = "Request Timeout";
	_status_code["409"] = "Conflict";
	_status_code["410"] = "Gone";
	_status_code["411"] = "Length Required";
	_status_code["412"] = "Precondition Failed";
	_status_code["413"] = "Request Entity Too Large";
	_status_code["414"] = "Request URI Too Long";
	_status_code["415"] = "Unsupported Media Type";
	_status_code["416"] = "Requested Range Not Satisfiable";
	_status_code["500"] = "Expectation Failed";
	_status_code["501"] = "Internal Server Error";
	_status_code["502"] = "Not Implemented";
	_status_code["503"] = "Bad Gateway";
	_status_code["504"] = "Gateway Timeout";
	_status_code["505"] = "HTTP Version Not Supported";
}
