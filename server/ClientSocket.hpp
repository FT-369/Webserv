#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP
#include "Socket.hpp"
#include "Response.hpp"
#include "Request.hpp"

class ClientSocket : public Socket
{
private:
	Response *response;
	Request *request;
	// Status status;
	// unsigned long start_time;
	ConfigServer server_info;

private:
	ClientSocket();

public:
	ClientSocket(int fd, ConfigServer server_info);
	ConfigServer getConnectServerInfo();
	~ClientSocket();
	Request* getRequest();
	Response* getResponse();
	int recieveRequest();
	Status getRequestStatus();
};

#endif
