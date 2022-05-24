#include "Response.hpp"

Response::Response(Request *request, Resource *resource)
	: _request(request), _resource(resource), _socket_write(NULL)
{
	_socket_write = fdopen(dup(_request->getSocketFD()), "w");
}

Response::~Response()
{
}

FILE* Response::getSocketWriteFD() const { return _socket_write; }

void Response::setStatusCode(std::string const &status_code) { _status_code = status_code; }
void Response::setEntity(std::string const &entity) { _entity = entity; }

void Response::makeStartLine()
{
	_start_line = _request->_protocol + " " + _status_code + " " + GlobalConfig::getStatusCode()[_status_code] + "\r\n";
}

void Response::makeHeader()
{
	_header["Server"] = "Mac Web Server";
}

void Response::makeGetHeader()
{
	_header["Content-type"] = _resource->getSrcContentType();
	_header["Content-length"] = std::to_string(_entity.length());
	_header["Server"] = "Mac Web Server";
}

void Response::makeRedirectHeader()
{
	if (_request->getRoute()->getReturnCode() == "")
		return;
	_status_code = _request->getRoute()->getReturnCode();
	_header["Location"] = _request->getRoute()->getReturnData();
}

void Response::makeResponse()
{
	// resource의 content 가져오기
	_entity = _resource->getContent();

	std::vector<std::string> temp;
	temp = _request->getRoute()->getCommonDirective()._limit_except;
	std::cout << "[Mapping Path] url: " << _request->getRoute()->getUrl() << ", file:" << _request->getFile() << std::endl;

	if (find(temp.begin(), temp.end(), _request->getMethod()) == temp.end())
	{
		std::cout << "ERROR Response " << std::endl;
		makeHeader();
	}
	else if (_request->getMethod() == "GET")
	{
		std::cout << "GET" << std::endl;
		makeGetHeader();
	}
	else if (_request->getMethod() == "POST")
	{
		std::cout << "POST" << std::endl;
		makeHeader();
	}
	else if (_request->getMethod() == "DELETE")
	{
		std::cout << "DELETE" << std::endl;
		makeHeader();
	}
	makeRedirectHeader();
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

// void Response::makeEntity(std::string file)
// {
// 	std::string buffer;
// 	std::string filePath;
// 	std::ifstream is(file);

// 	if (is.fail())
// 	{
// 		makeErrorResponse("404");
// 		return; // throw error
// 	}
// 	while (std::getline(is, buffer))
// 	{
// 		_entity += buffer + "\n";
// 	}
// 	_status_code = "200";
// }

// void Response::makeResponse()
// {
// 	std::vector<std::string> temp;

// 	temp = _request->getRoute()->getCommonDirective()._limit_except;
// 	std::cout << "[Mapping Path] url: " << _request->getRoute()->getUrl() << ", file:" << _request->getFile() << std::endl;

// 	if (find(temp.begin(), temp.end(), _request->getMethod()) == temp.end())
// 	{
// 		std::cout << "ERROR Response " << std::endl;
// 		makeErrorResponse("405");
// 	}
// 	else if (_request->getFile().find(".php") != std::string::npos) {
// 		CgiHandler cgi(_request->getRoute(), _request);
// 		cgi.executeCgi();
// 		return ;
// 	}
// 	else if (_request->getMethod() == "GET")
// 	{
// 		std::cout << "GET" << std::endl;
// 		makeGetResponse();
// 	}
// 	else if (_request->getMethod() == "POST")
// 	{
// 		std::cout << "POST" << std::endl;
// 		makePostResponse();
// 	}
// 	else if (_request->getMethod() == "DELETE")
// 	{
// 		std::cout << "DELETE" << std::endl;
// 		makeDeleteResponse();
// 	}
// 	setRedirect();
// 	makeHeader();
// 	makeStartLine();
// }

// void Response::makeAutoIndex(std::string directory, DIR *dir)
// {
// 	std::string host = "http://" + _request->getRequestHeader()["Host"];
// 	std::string pos = directory[directory.size() - 1] == '/' ? directory : directory + "/";

// 	_entity += "<!DOCTYPE html>\n";
// 	_entity += "<html>\n";
// 	_entity += "<head>\n</head>\n";
// 	_entity += "<body>\n";
// 	_entity += "<h1> Index of "+ pos + "</h1>\n";

// 	if (dir == NULL)
// 		dir = opendir(_request->getRoute()->getCommonDirective()._root.c_str());

// 	struct dirent *file = NULL;
// 	while ((file = readdir(dir)) != NULL) {
// 		std::string d_name = file->d_type == DT_DIR ? std::string(file->d_name) + "/" : std::string(file->d_name);
// 		_entity += "<a href=\"" + host + pos + file->d_name + "\">";
// 		_entity += file->d_name;
// 		if (file->d_type == DT_DIR)
// 			_entity += + "/";
// 		_entity += "</a><br>\n";
// 	}
// 	closedir(dir);

// 	_entity += "</body>\n";
// 	_entity += "</html>\n";
// }

// void Response::makeGetResponse()
// {
// 	std::string entityFile;
// 	std::string root = _request->getRoute()->getCommonDirective()._root;
// 	std::vector<std::string> indexPage = _request->getRoute()->getCommonDirective()._index;

// 	if (_request->getFile() == "")
// 	{
// 		if (!indexPage.empty())
// 		{
// 			for (size_t i = 0; i < indexPage.size(); i++)
// 			{
// 				std::string indexfile = root + "/" + indexPage[i];
// 				if (isFile(indexfile) == 1)
// 				{
// 					int resource_fd = open(indexfile.c_str(), O_RDONLY);
// 					// socket list에 new Resource 해서 추가
// 					// addEvent
// 					// return;
// 				}
// 				// return; 실패
// 				// std::ifstream idx(root + "/" + indexPage[i]);
// 				// if (idx.is_open())
// 				// {
// 				// 	return makeEntity(entityFile = root + "/" + indexPage[i]);
// 				// }
// 			}
// 		}
// 		else if (_request->getRoute()->getCommonDirective()._autoindex)
// 		{
// 			return makeAutoIndex("/", 0);
// 		}
// 	}
// 	else
// 	{
// 		DIR* isDir = 0;
// 		std::ifstream is(root + _request->getFile());

// 		if (!is.fail()) // 존재하는 파일 or 디렉토리
// 		{
// 			entityFile = root + _request->getFile();
// 			if ((isDir = opendir(entityFile.c_str())) == NULL) // 파일인 경우
// 			{
// 				// if (isCGI == true)
// 				// {
// 				// }
// 				if (isFile(entityFile) == 1)
// 				{
// 					int resource_fd = open(entityFile.c_str(), O_RDONLY);
// 					// socket list에 new Resource 해서 추가
// 					// addEvent
// 				}
// 				return;
// 				// return makeEntity(entityFile);
// 			}
// 			else if (_request->getRoute()->getCommonDirective()._autoindex) // 디렉토리인데 autoindex가 켜져있으면
// 			{
// 				return makeAutoIndex(_request->getFile(), isDir);
// 			}
// 		}
// 		else if (_request->getRoute()->getCommonDirective()._autoindex)
// 		{
// 			return makeAutoIndex("/", 0);
// 		}
// 	}
// 	return makeErrorResponse("404");
// }

// void	Response::makePostResponse()
// {
// 	std::string path = _request->getRoute()->getCommonDirective()._root;
// 	std::string dir, dirpath, filename;
// 	size_t rpos = _request->getFile().rfind("/");

// 	if (rpos != std::string::npos)
// 		dir = _request->getFile().substr(0, rpos);
// 	dir == "" ? dirpath = path : dirpath = dir;
// 	if (_request->getFile() == "")
// 	{
// 		filename = dirpath + "/NewFile";
// 		std::string temp = filename;
// 		int i = 0;
// 		while (isDirectory(filename) || isFile(filename))
// 		{
// 			temp = filename + '(' + std::to_string(i) + ')';
// 			i++;
// 		}
// 		filename = temp;
// 	}
// 	else
// 	{
// 		filename = dirpath + _request->getFile();
// 	}
// 	// creatfilename();
// 	if (isDirectory(filename))
// 	{
// 		_status_code = "400";
// 		//makeErrorPage()
// 		return;
// 	}
// 	if (isFile(filename))
// 	{
// 		// if (isCGI() == true)
// 		// {
// 		// }
// 		std::ofstream ofs;
// 		ofs.open(filename, std::ios::app);
// 		if (ofs.is_open())
// 			_status_code = "200";
// 		else
// 		{
// 			_status_code = "403";
// 			// makeErrorPage();
// 			return ;
// 		}
// 		ofs << _request->_request_body;
// 		ofs.close();
// 	}
// 	else
// 	{
// 		std::ofstream ofs(filename);
// 		if (ofs.fail())
// 		{
// 			_status_code = "500";
// 			// makeErrorPage();
// 			return ;
// 		}
// 		else
// 			_status_code = "201";
// 		ofs << _request->_request_body;
// 		ofs.close();
// 	}
// 	// makeEntity(settingRoute()); entity가 없다 post는
// 	makePostHeader();
// 	makeStartLine();
// }

// void Response::makeDeleteResponse()
// {
// 	std::string entityFile;
// 	std::string root = _request->getRoute()->getCommonDirective()._root;
// 	std::vector<std::string> indexPage = _request->getRoute()->getCommonDirective()._index;

// 	if (_request->getFile() == "")
// 	{
// 		if (!indexPage.empty())
// 		{
// 			for (size_t i = 0; i < indexPage.size(); i++)
// 			{
// 				std::string indexfile = root + "/" + indexPage[i];
// 				if (isFile(indexfile) == 1)
// 				{
// 					return ; // 성공 응답
// 				}
// 			}
// 		}
// 	}
// 	else
// 	{
// 		entityFile = root + _request->getFile();
// 		if (isFile(entityFile) == 1)
// 		{
// 			return ; // 성공 응답
// 		}
// 		else if (isDirectory(entityFile) == 1)
// 		{
// 			return ; // 뭐라고 응답?
// 		}
// 	}
// 	return ;	// 실패 응답
// }

// void Response::makeErrorResponse(std::string error_num)
// {
// 	std::string root = "./static_file/defaultErrorPage.html";
// 	std::map<std::string, std::string> temp = _request->getRoute()->getCommonDirective()._error_page;
// 	std::string buffer;

// 	_status_code = error_num;
// 	if (temp.find(_status_code) != temp.end())
// 	{
// 		root =_request->getRoute()->getCommonDirective()._root + "/" + _request->getRoute()->getCommonDirective()._error_page[_status_code];
// 	}
// 	std::ifstream idx(root);
// 	if (idx.is_open() == false)
// 	{
// 		_entity = "";
// 		return ;
// 	}
// 	while (std::getline(idx, buffer))
// 	{
// 		_entity += buffer + "\n";
// 	}
// }
