#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "utils.hpp"
#include "Resource.hpp"
#include "ConfigLocation.hpp"

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
	const int _socket_fd;
	FILE *_socket_read;
	RequestStage _stage;
	std::string _method;
	std::string _path;
	std::string _query;
	std::string _protocol;
	std::string _request_body;
	std::map<std::string, std::string> _request_header;
	ConfigLocation *_route;
	std::string _file; // 라우팅 경로를 제외한 파일  // _route->getUrl() + _file = _path;

public:
	Request(int socket_fd);
	~Request();
	int parseRequest();
	void setRoute(std::vector<ConfigLocation> const &locations);

	int getSocketFD() const;
	FILE *getSocketReadFP() const;
	FILE *getSocketWriteFP() const;
	std::string getMethod() const;
	std::string getPath() const;
	std::string getQuery() const;
	std::string getPort() const;
	std::string getServerName() const;
	std::string getContentType() const;
	std::string getProtocol() const;
	std::string getRequestBody() const;
	std::map<std::string, std::string> getRequestHeader() const;
	RequestStage getRequestStage() const;
	ConfigLocation *getRoute() const;
	ConfigLocation *getRoute();
	std::string getFile() const;

private:
	int parseRequestLine();
	int parseRequestHeader();
	int parseRequestBody();
	std::string ft_fgets_line(FILE *fp);
};

#endif
