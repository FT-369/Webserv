#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP
#include "Socket.hpp"
#include <cstring>

class ServerSocket : public Socket
{
private:
	struct sockaddr_in addr;

public:
	ServerSocket();
	ServerSocket(ConfigServer server);
	~ServerSocket();
	int binding();
	int clientAccept(int &connectFD);
};



#endif
