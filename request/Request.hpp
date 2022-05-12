#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "utils.hpp"
#include <stdio.h>
#include <unistd.h>
#include <map>

#define GET_LINE_BUF 1024

enum Status
{
	READ_REQUEST_LINE,
	READ_REQUEST_HEADER,
	READ_REQUEST_BODY,
	READ_END_OF_REQUEST,
};

class Request
{
public:
	const int _socket_fd;
	FILE *_socket_read;
	Status _status;
	// FILE* socket_write;

	std::string _method;
	std::string _path;
	std::map<std::string, std::string> _query;
	std::string _protocol;
	std::string _request_body;
	std::map<std::string, std::string> _request_header;

public:
	Request(int socket_fd);
	~Request();
	int parseRequest();

	int getSocketFD() const;
	FILE *getSocketReadFP() const;
	FILE *getSocketWriteFP() const;
	std::string getMethod() const;
	std::string getPath() const;
	std::map<std::string, std::string> getQuery() const;
	std::string getProtocol() const;
	std::string getRequestBody() const;
	std::map<std::string, std::string> getRequestHeader() const;
	Status getStatus();

private:
	int parseRequestLine();
	int parseRequestHeader();
	int parseRequestBody();
	std::string ft_fgets_line(FILE *fp);
};

#endif
