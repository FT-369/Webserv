#ifndef SOCKET_CPP
#define SOCKET_CPP
#include "ConfigServer.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

#define SERVER_SOCKET 1
#define CLLIENT_SOCKET 2

class Socket
{
protected:
	int	socket_fd;
	int	socket_type;
	int	port;
	std::string host;

public:
	Socket();
	virtual ~Socket();
	int getSocketFd();
	int getSocketPort();
	std::string getSocketHost();
	int getSocketType();
};



#endif
