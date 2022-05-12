#include "Request.hpp"

Request::Request(int socket_fd)
	: _socket_fd(socket_fd), _socket_read(fdopen(socket_fd, "r")), _status(READ_REQUEST_LINE)
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
// FILE* Request::getSocketWriteFP() const { return socket_write; }
std::string Request::getMethod() const { return _method; }
std::string Request::getPath() const { return _path; }
std::map<std::string, std::string> Request::getQuery() const { return _query; }
std::string Request::getProtocol() const { return _protocol; }
std::string Request::getRequestBody() const { return _request_body; }
std::map<std::string, std::string> Request::getRequestHeader() const { return _request_header; }
Status Request::getStatus() { return _status; }

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

int Request::parseRequestLine()
{
	std::string get_line;
	std::vector<std::string> request_line;
	std::vector<std::string> query_list;
	size_t pos;

	get_line = ft_fgets_line(getSocketReadFP());
	if (get_line == "" || get_line == "\r\n")
		return ERROR;

	request_line = ft_split_space(get_line);
	if (request_line.size() != 3)
		return ERROR;

	_method = request_line[0];
	if ((pos = request_line[1].find("?")) == std::string::npos)
	{
		_path = request_line[1];
	}
	else
	{
		_path = request_line[1].substr(0, pos);
		query_list = ft_split(request_line[1].substr(pos + 1), "&");
		for (size_t i = 0; i < query_list.size(); i++)
		{
			if ((pos = query_list[i].find("=")) == std::string::npos)
			{
				_query[query_list[i]] = "";
				// return ERROR;
			}
			else
			{
				_query[query_list[i].substr(0, pos)] = query_list[i].substr(pos + 1);
			}
		}
	}
	_protocol = request_line[2];

	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return ERROR;
	if (_path[0] != '/')
		return ERROR;
	if ((pos = _protocol.find("HTTP/")) == std::string::npos)
		return ERROR;

	_status = READ_REQUEST_HEADER;
	return SUCCESS;
}

int Request::parseRequestHeader()
{
	std::string get_line, key, value;
	std::vector<std::string> key_value;

	get_line = ft_fgets_line(getSocketReadFP());
	if (get_line == "" || get_line == "\r\n")
	{
		_status = READ_REQUEST_BODY;
		return SUCCESS;
	}

	key_value = ft_split(get_line, ": ");

	if (key_value.size() != 2)
	{
		_request_header.clear();
		return ERROR;
	}

	key = key_value[0];
	value = key_value[1].replace(key_value[1].find("\r\n"), 2, "\0");
	_request_header[key] = value;
	return SUCCESS;
}

int Request::parseRequestBody()
{
	char line[GET_LINE_BUF];

	if (fgets(line, GET_LINE_BUF, getSocketReadFP()) == NULL)
	{
		_status = READ_END_OF_REQUEST;
	}
	else
	{
		_request_body += (std::string(line));
	}
	return SUCCESS;
}

int Request::parseRequest()
{
	switch (_status)
	{
	case READ_REQUEST_LINE:
		if (parseRequestLine() == ERROR)
			return ERROR;
		break;

	case READ_REQUEST_HEADER:
		if (parseRequestHeader() == ERROR)
			return ERROR;
		break;

	case READ_REQUEST_BODY:
		if (parseRequestBody() == ERROR)
			return ERROR;
		break;

	default:
		break;
	}
	return SUCCESS;
}
