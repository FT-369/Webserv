#ifndef SERVERSOCKET_CPP
#define SERVERSOCKET_CPP
#include "Socket.hpp"
#include <cstring>

class ServerSocket : public Socket
{
private:
	struct sockaddr_in addr;

public:
	ServerSocket();
	~ServerSocket();
	int binding();
	int clientAccept(int &connectFD);
};



#endif
