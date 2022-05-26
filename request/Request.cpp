#include "Request.hpp"

Request::Request(int socket_fd)
	: _socket_fd(socket_fd), _socket_read(fdopen(socket_fd, "r")), _stage(READ_REQUEST_LINE)
{
}

Request::~Request()
{
	fclose(_socket_read);
	// fclose(socket_write);
	// close(_socket_fd);
}

int Request::getSocketFD() const { return _socket_fd; }
FILE *Request::getSocketReadFP() const { return _socket_read; }

std::string Request::getPort() const {
	size_t pos = getRequestHeader()["Host"].find(":");
	if (pos == std::string::npos) {
		return "80";
	} else {
		return getRequestHeader()["Host"].substr(pos + 1);
	}
}

std::string Request::getServerName() const {
	size_t pos = getRequestHeader()["Host"].find(":");
	if (pos == std::string::npos) {
		return getRequestHeader()["Host"];
	} else {
		return getRequestHeader()["Host"].substr(0, pos);
	}
}

std::string Request::getContentType() const
{
	if (_request_header.find("Content-Type") == _request_header.end()) {
		return "text/plain";
	}
	return getRequestHeader()["Content-Type"];
}

std::string Request::getMethod() const { return _method; }
std::string Request::getPath() const { return _path; }
std::string Request::getQuery() const { return _query; }
std::string Request::getProtocol() const { return _protocol; }
std::string Request::getRequestBody() const { return _request_body; }
std::map<std::string, std::string> Request::getRequestHeader() const { return _request_header; }
RequestStage Request::getRequestStage() const { return _stage; }
ConfigLocation *Request::getRoute() const { return _route; }
ConfigLocation *Request::getRoute() { return _route; }
std::string Request::getFile() const { return _file; }

std::string Request::ft_fgets_line(FILE *fp)
{
	char line[GET_LINE_BUF] = {
		0,
	};
	std::string getline;
	size_t len;

	if (!fgets(line, GET_LINE_BUF, fp))
		return getline;
	getline = std::string(line);

	while ((len = std::strlen(line)) && line[len - 1] != '\n')
	{
		if (!fgets(line, GET_LINE_BUF, fp))
			return getline;
		getline += std::string(line);
	}
	return getline;
}

// int Request::parseRequest()
// {
// 	if (parseRequestLine() == ERROR)
// 		return ERROR;
// 	if (parseRequestHeader() == ERROR)
// 		return ERROR;
// 	if (this->method == "POST")
// 		parseRequestBody();
// 	std::cout << "!!! path of requtest !!! : " << this->getPath() << std::endl;
// 	printReq("REQUEST Query", this->getQuery());
// 	printReq("REQUEST Header", this->getRequestHeader());
// 	return SUCCESS;
// }

// int Request::parseRequestLine()
// {
// 	std::string get_line;
// 	std::string get_line;
// 	std::vector<std::string> request_line;
// 	std::vector<std::string> query_list;
// 	size_t pos;

// 	get_line = ft_fgets_line(getSocketReadFP());
// 	if (get_line == "" || get_line == "\r\n")
// 		return ERROR;

// 	request_line = ft_split_space(get_line);
// 	if (request_line.size() != 3)
// 		return ERROR;

// 	method = request_line[0];
// 	if ((pos = request_line[1].find("?") ) == std::string::npos) {
// 		path = request_line[1];
// 	} else {
// 		path = request_line[1].substr(0, pos);
// 		query_list = ft_split(request_line[1].substr(pos + 1), "&");
// 		for (size_t i = 0; i < query_list.size(); i++)
// 		{
// 			if ((pos = query_list[i].find("=")) == std::string::npos) {
// 				query[query_list[i]] = "";
// 				// return ERROR;
// 			} else {
// 				query[query_list[i].substr(0, pos)] = query_list[i].substr(pos + 1);
// 			}
// 		}
// 	}
// 	protocol = request_line[2];

// 	if (method != "GET" && method != "POST" && method != "DELETE")
// 		return ERROR;
// 	if (path[0] != '/')
// 		return ERROR;
// 	if ((pos = protocol.find("HTTP/")) == std::string::npos)
// 		return ERROR;

// 	return SUCCESS;
// }

// int Request::parseRequestHeader()
// {
// 	char line[GET_LINE_BUF];
// 	std::string get_line, key, value;
// 	std::vector<std::string> key_value;

// 	get_line = ft_fgets_line(getSocketReadFP());
// 	while (get_line != "" && get_line != "\r\n")
// 	{
// 		key_value = ft_split(get_line, ": ");

// 		if (key_value.size() != 2) {
// 			request_header.clear();
// 			return ERROR;
// 		}

// 		key = key_value[0];
// 		value = key_value[1].replace(key_value[1].find("\r\n"), 2, "\0");
// 		request_header[key] = value;

// 		get_line = ft_fgets_line(getSocketReadFP());
// 	}
// 	if (request_header.find("Host") == request_header.end())
// 		return ERROR; // 더

// 	return SUCCESS;
// }

// int Request::parseRequestBody()
// {
// 	char line[GET_LINE_BUF];

// 	while (fgets(line, GET_LINE_BUF, getSocketReadFP()) != NULL)
// 	{
// 		request_body += (std::string(line));
// 	}
// 	return SUCCESS;
// }

void Request::parseRequestLine()
{
	std::string get_line;
	std::vector<std::string> request_line;
	std::vector<std::string> query_list;
	size_t pos;

	get_line = ft_fgets_line(getSocketReadFP());
	_request_main += get_line;
	if (get_line == "" || get_line == "\r\n")
		throw request_error("Invalid Request");

	request_line = ft_split_space(get_line);
	if (request_line.size() != 3)
		throw request_error("Invalid request line");

	_method = request_line[0];
	if ((pos = request_line[1].find("?")) == std::string::npos)
	{
		_path = request_line[1];
	}
	else
	{
		_path = request_line[1].substr(0, pos);
		_query = request_line[1].substr(pos + 1);
	}
	_protocol = request_line[2];

	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		throw request_error("Unknown method");
	if (_path[0] != '/')
		throw request_error("Invalid path");
	if ((pos = _protocol.find("HTTP/")) == std::string::npos)
		throw request_error("not http protocol");

	_stage = READ_REQUEST_HEADER;
}

void Request::parseRequestHeader()
{
	std::string get_line, key, value;
	std::vector<std::string> key_value;

	get_line = ft_fgets_line(getSocketReadFP());
		_request_main += get_line;

	if (get_line == "" || get_line == "\r\n")
	{
		_stage = READ_REQUEST_BODY;
		return;
	}

	key_value = ft_split(get_line, ": ");

	if (key_value.size() != 2)
	{
		_request_header.clear();
		throw request_error("request format");
	}

	key = key_value[0];
	value = key_value[1].replace(key_value[1].find("\r\n"), 2, "\0");
	_request_header[key] = value;
}

void Request::parseRequestBody()
{
	char line[GET_LINE_BUF];

	if (fgets(line, GET_LINE_BUF, getSocketReadFP()) == NULL)
	{
		_stage = READ_END_OF_REQUEST;
	}
	else
	{
		_request_body += (std::string(line));
		_request_main += (std::string(line));
	}
}

void Request::parseRequest()
{
	switch (_stage)
	{
	case READ_REQUEST_LINE:
		parseRequestLine();
		break;

	case READ_REQUEST_HEADER:
		parseRequestHeader();
		break;

	case READ_REQUEST_BODY:
		parseRequestBody();
		break;

	default:
		break;
	}
}

void Request::setRoute(std::vector<ConfigLocation> const &locations)
{
	int path_len = _path.size();
	std::cout << "setRoute path: " << _path << std::endl;
	_file = "";

	for (int i = path_len - 1; i >= -1; i--)
	{
		if (i == path_len - 1 || _path[i + 1] == '/')
		{
			for (int j = 0; j < locations.size(); j++)
			{
				if (_path.substr(0, i + 1) == locations[j].getUrl() || _path.substr(0, i + 1) == locations[j].getUrl() + "/")
				{
					_route = new ConfigLocation(locations[j].getUrl(), locations[j].getCommonDirective(), locations[j].getReturnCode(),
					locations[j].getReturnData());
					if (i != path_len - 1)
						_file = _path.substr(i + 1);
					return;
				}
				else if (i == -1 && locations[j].getUrl() == "/")
				{
					_route = new ConfigLocation("/", locations[j].getCommonDirective(), locations[j].getReturnCode(),
					locations[j].getReturnData());
					_file = _path;
					return;
				}
			}
		}
	}
}
std::string Request::getRequestMain() const 
{
	return _request_main;
}
