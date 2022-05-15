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

void Response::makeAutoIndex(std::string directory, DIR *dir)
{
	std::string host = "http://" + _request->getRequestHeader()["Host"];
	std::string pos = directory[directory.size() - 1] == '/' ? directory : directory + "/";

	_entity += "<!DOCTYPE html>\n";
	_entity += "<html>\n";
	_entity += "<head>\n</head>\n";
	_entity += "<body>\n";
	_entity += "<h1> Index of "+ pos + "</h1>\n";

	if (dir == NULL)
		dir = opendir(_route->getCommonDirective()._root.c_str());

	struct dirent *file = NULL;
	while ((file = readdir(dir)) != NULL) {
		std::string d_name = file->d_type == DT_DIR ? std::string(file->d_name) + "/" : std::string(file->d_name);
		_entity += "<a href=\"" + host + pos + file->d_name + "\">";
		_entity += file->d_name;
		if (file->d_type == DT_DIR)
			_entity += + "/";
		_entity += "</a><br>\n";
	}
	closedir(dir);

	_entity += "</body>\n";
	_entity += "</html>\n";
}

void Response::settingRoute()
{
	std::string entityFile;
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
					return makeEntity(entityFile = root + "/" + indexPage[i]);
				}
			}
		}
		else if (_route->getCommonDirective()._autoindex)
		{
			return makeAutoIndex("/", 0);
		}
	}
	else
	{
		DIR* isDir = 0;
		std::ifstream is(root + _file);

		if (!is.fail()) // 존재하는 파일 or 디렉토리
		{
			entityFile = root + _file;
			if ((isDir = opendir(entityFile.c_str())) == NULL) // 파일인 경우
			{
				return makeEntity(entityFile);
			}
			else if (_route->getCommonDirective()._autoindex) // 디렉토리인데 autoindex가 켜져있으면
			{
				return makeAutoIndex(_file, isDir);
			}
		}
		else if (_route->getCommonDirective()._autoindex)
		{
			return makeAutoIndex("/", 0);
		}
	}
	// return makeEntity("./static_file/404.html");
	return makeErrorResponse("404");
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
	settingRoute();
}

void Response::makeDeleteResponse()
{
	settingRoute();
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
