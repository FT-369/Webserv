#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "utils.hpp"
#include "Resource.hpp"
#include "ConfigLocation.hpp"
#include "webserv.hpp"

#define GET_LINE_BUF 1024

enum RequestStage
{
	READ_REQUEST_LINE,
	READ_REQUEST_HEADER,
	READ_REQUEST_BODY,
	READ_END_OF_REQUEST,
};

class Request
{
public:
	FILE *_socket_read;
	RequestStage _stage;
	std::string _method;
	std::string _path;
	std::string _query;
	std::string _protocol;
	std::string _request_body;
	std::string _request_main;
	unsigned int _request_line_size;
	unsigned int _request_header_size;
	unsigned long _request_body_size;
	std::map<std::string, std::string> _request_header;

public:
	Request(int socket_fd);
	~Request();
	void parseRequest();

	FILE *getSocketReadFP() const;
	std::string getMethod() const;
	std::string getPath() const;
	std::string getQuery() const;
	std::string getPort() const;
	std::string getServerName() const;
	std::string getContentType() const;
	std::string getProtocol() const;
	std::string getRequestBody() const;
	std::string getRequestMain() const;
	std::map<std::string, std::string> getRequestHeader() const;
	RequestStage getRequestStage() const;
	unsigned int getRequestHeaderSize() const;
	unsigned long getRequestBodySize() const;

private:
	void parseRequestBody();
	void parseRequestHeader(std::string const &line);
	void parseRequestLine(std::string const &line);
};

#endif
