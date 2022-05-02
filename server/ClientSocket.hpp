#ifndef CLIENTSOCKET_CPP
#define CLIENTSOCKET_CPP
#include "Socket.hpp"

class ClientSocket : public Socket
{
private:
	// Response response;
	// Request request;
	// Status status;
	// unsigned long start_time;

public:
	ClientSocket();
	~ClientSocket();
	int binding();
	int acceptClient();
}

#endif
