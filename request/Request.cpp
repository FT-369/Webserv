#include "Request.hpp"

Request::Request(int socket_fd)
	: socket_fd(socket_fd), socket_read(fdopen(socket_fd, "r"))
{
}

Request::~Request()
{
	fclose(socket_read);
	// fclose(socket_write);
	// close(_socket_fd);
}

int Request::getSocketFD() const { return socket_fd; }
FILE* Request::getSocketReadFP() const { return socket_read; }
// FILE* Request::getSocketWriteFP() const { return socket_write; }
std::string Request::getMethod() const { return method; }
std::string Request::getPath() const { return path; }
std::string Request::getProtocol() const { return protocol; }
std::string Request::getRequestBody() const { return request_body; }
std::map<std::string, std::string> Request::getRequestHeader() const { return request_header; }

std::string Request::ft_fgets_line(FILE* fp)
{
	char line[GET_LINE_BUF] = { 0, };
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

int Request::parseRequest()
{
	if (parseRequestLine() == ERROR)
		return ERROR;
	if (parseRequestHeader() == ERROR)
		return ERROR;
	if (this->method == "POST")
		parseRequestBody();
	return SUCCESS;
}

int Request::parseRequestLine()
{
	char line[GET_LINE_BUF];
	std::string get_line;
	std::vector<std::string> request_line;
	size_t pos;

	get_line = ft_fgets_line(getSocketReadFP());
	if (get_line == "" || get_line == "\r\n")
		return ERROR;

	request_line = ft_split(get_line, " ");
	if (request_line.size() != 3)
		return ERROR;

	method = request_line[0];
	path = request_line[1];
	protocol = request_line[2];
	
	if (method != "GET" && method != "POST" && method != "DELETE")
		return ERROR;
	if (path[0] != '/')
		return ERROR;
	if ((pos = protocol.find("HTTP/")) == std::string::npos)
		return ERROR;

	return SUCCESS;
}

int Request::parseRequestHeader()
{
	char line[GET_LINE_BUF];
	std::string get_line, key, value;
	std::vector<std::string> key_value;

	get_line = ft_fgets_line(getSocketReadFP());
	while (get_line != "" && get_line != "\r\n")
	{
		key_value = ft_split(get_line, ": ");

		if (key_value.size() != 2) {
			request_header.clear();
			return ERROR;
		}

		key = key_value[0];
		value = key_value[1].replace(key_value[1].find("\r\n"), 2, "\0");
		request_header[key] = value;

		get_line = ft_fgets_line(getSocketReadFP());
	}
	if (request_header.find("Host") == request_header.end())
		return ERROR; // 더 
	
	return SUCCESS;
}

int Request::parseRequestBody()
{
	char line[GET_LINE_BUF];

	while (fgets(line, GET_LINE_BUF, getSocketReadFP()) != NULL)
	{
		request_body += (std::string(line));
	}
	return SUCCESS;
}
