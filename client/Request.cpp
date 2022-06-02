#include "Request.hpp"

Request::Request(int socket_fd)
	: _socket_read(fdopen(dup(socket_fd), "r")), _stage(READ_REQUEST_LINE), _request_header_size(0), _request_body_size(0)
{
}

Request::~Request()
{
	fclose(_socket_read);
}

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

void Request::parseRequestLine(std::string const &line)
{
	std::vector<std::string> request_line;
	size_t pos;

	if (line == "")
		throw request_error("Invalid Request Line");

	std::cerr << line << std::endl;
	request_line = ft_split_space(line);
	if (request_line.size() != 3)
		throw request_error("Invalid request Line");

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

	if (_path == "" && _path[0] != '/')
		throw request_error("Invalid path");
	if ((pos = _protocol.find("HTTP")) == std::string::npos)
		throw request_error("not http protocol");
}

void Request::parseRequestHeader(std::string const &line)
{
	std::vector<std::string> headers;
	std::vector<std::string> key_value;

	headers = ft_split(line, "\r\n");

	for (size_t i = 0; i < headers.size(); i++)
	{
		if (headers[i] == "")
		{
			throw request_error("Invalid Request header");
		}
		key_value = ft_split(headers[i], ": ");
		if (key_value.size() != 2)
		{
			throw request_error("Invalid Request header format");
		}
		_request_header[key_value[0]] = key_value[1];
	}
}

void Request::parseRequestBody()
{
	std::vector<std::string> chunk;
	std::string chunk_size_hexa;
	std::string chunk_message;
	unsigned int chunk_size;
	std::stringstream convert;

	if ((_request_header.find("Transfer-Encoding") != _request_header.end()
		&& _request_header["Transfer-Encoding"] == "chunked") || 
		(_request_header.find("transfer-encoding") != _request_header.end()
		&& _request_header["transfer-encoding"] == "chunked"))
	{
		chunk = ft_split(_request_main, "\r\n");

		for (size_t i = 0; i < chunk.size(); i += 2)
		{
			chunk_size_hexa = chunk[i];
			std::stringstream convert(chunk_size_hexa);
			convert >> std::hex >> chunk_size;
			if (chunk_size == 0)
				break;
			chunk_message = chunk[i + 1];
			_request_body.append(chunk_message, chunk_message.length());
			_request_body_size += chunk_message.length();
		}
	}
	else
	{
		_request_body = _request_main;
		_request_body_size = _request_main.length();
	}
}

void Request::parseRequest()
{
	char line[GET_LINE_BUF];
	
	memset(line, 0, GET_LINE_BUF);
	long fread_ret = fread(line, sizeof(char), GET_LINE_BUF - 1, getSocketReadFP());
	if (fread_ret < 0)
	{
		throw request_error("request fread error");
	}
	if (fread_ret == 0 && _stage == READ_END_OF_REQUEST)
	{
		return;
	}

	line[fread_ret] = 0;
	_request_main.append(line, fread_ret);
	if (_stage == READ_REQUEST_LINE)
	{
		size_t line_end = _request_main.find("\r\n");
		if (line_end != std::string::npos)
		{
			parseRequestLine(_request_main.substr(0, line_end));
			_request_line_size = line_end;
			_stage = READ_REQUEST_HEADER;
			_request_main = _request_main.substr(line_end + 2);
		}
	}
	else if (_stage == READ_REQUEST_HEADER)
	{
		size_t header_end = _request_main.find("\r\n\r\n");
		if (header_end != std::string::npos)
		{
			parseRequestHeader(_request_main.substr(0, header_end));
			_request_header_size = header_end;
			_stage = READ_REQUEST_BODY;
			_request_main = _request_main.substr(header_end + 4);
		}
	}
	else if (_stage == READ_REQUEST_BODY)
	{
		if (fread_ret == 0)
		{
			parseRequestBody();
			_stage = READ_END_OF_REQUEST;
		}
	}
}
