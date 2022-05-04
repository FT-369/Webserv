#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP
#include "Socket.hpp"
// #include "Response.hpp"
#include "Request.hpp"

class ClientSocket : public Socket
{
private:
	// Response response;
	// Request request;
	// Status status;
	// unsigned long start_time;

public:
	ClientSocket();
	ClientSocket(int fd);
	~ClientSocket();
	
};

#endif
