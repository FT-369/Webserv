#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP
#include "Socket.hpp"
#include "Response.hpp"
#include "Request.hpp"

class ClientSocket : public Socket
{
private:
	Response *_response;
	Request *_request;
	// Status status;
	// unsigned long start_time;
	ConfigServer _server_info;

private:
	ClientSocket();

public:
	ClientSocket(int fd, ConfigServer server_info);
	ConfigServer getConnectServerInfo();
	~ClientSocket();
	Request *getRequest() const;
	Response *getResponse() const;
	int recieveRequest();
	void sendResponse();
	Status getRequestStatus();
};

#endif
