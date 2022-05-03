#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP
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
};

#endif
