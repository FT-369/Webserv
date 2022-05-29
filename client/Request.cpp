#include "Request.hpp"

Request::Request(int socket_fd)
	: _socket_read(fdopen(socket_fd, "r")), _stage(READ_REQUEST_LINE), _request_header_size(0), _request_body_size(0)
{
}

Request::~Request()
{
	fclose(_socket_read);
	// fclose(socket_write);
	// close(_socket_fd);
}

// int Request::getSocketFD() const { return _socket_fd; }
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
	if ((pos = _protocol.find("HTTP/1.1")) == std::string::npos)
		throw request_error("not http protocol");

	_stage = READ_REQUEST_HEADER;
}

void Request::parseRequestHeader()
{
	std::string get_line, key, value;
	std::vector<std::string> key_value;

	get_line = ft_fgets_line(getSocketReadFP());
	_request_main += get_line;
	_request_header_size += get_line.size();
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
	memset(line, 0, GET_LINE_BUF);
	long fread_ret;
	std::string fgets_ret;

	fread_ret = fread(line, sizeof(char), GET_LINE_BUF - 1, getSocketReadFP());
	line[fread_ret] = 0;
	_request_body_size += fread_ret;
	if (fread_ret <= 0)
	{
		_stage = READ_END_OF_REQUEST;
	}
	else
	{
		_request_body.append(line, fread_ret);
		_request_main.append(line, fread_ret);
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

std::string Request::getRequestMain() const 
{
	return _request_main;
}

unsigned int Request::getRequestHeaderSize() const
{
	return _request_header_size;
}

unsigned long Request::getRequestBodySize() const
{
	return _request_body_size;
}
