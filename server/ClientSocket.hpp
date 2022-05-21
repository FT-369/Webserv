#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP
#include "Socket.hpp"
#include "Response.hpp"
#include "Request.hpp"

enum Stage
{
	GET_REQUEST,
	SET_RESOURCE,
	MAKE_RESPONSE,
	CGI_READ,
	CGI_WRITE,
	FILE_READ,
	FILE_WRITE,
	AUTOINDEX_WRITE,
	CLOSE,
};

class ClientSocket : public Socket
{
private:
	Request *_request;
	Response *_response;
	Resource *_resource;
	Stage _stage;
	// unsigned long start_time;
	ConfigServer _server_info;

private:
	ClientSocket();

public:
	ClientSocket(int fd, ConfigServer server_info);
	ConfigServer getConnectServerInfo();
	~ClientSocket();
	Request *getRequest() const;
	Resource *getResource() const;
	Response *getResponse() const;
	int recieveRequest();
	void sendResponse();
	RequestStage getRequestStage();
	std::string getErrorPage(std::string error_num);
	void setResourceFd();
	std::string getCgiType(std::string file);
	bool isCGI(const std::string &path);
};

#endif
