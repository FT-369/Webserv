#ifndef SOCKET_HPP
#define SOCKET_HPP
#include "ConfigServer.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

#define SERVER_SOCKET 1
#define CLIENT_SOCKET 2

class Socket
{
protected:
	int	socket_fd;
	int	socket_type;
	int	port;
	std::string host;

public:
	Socket();
	Socket(int socket_type);
	virtual ~Socket();
	int getSocketFd();
	int getSocketPort();
	std::string getSocketHost();
	int getSocketType();
};



#endif
