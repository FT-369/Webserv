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
	int _socket_fd;
	int _socket_type;
	int _port;
	std::string _host;

public:
	Socket();
	Socket(int socket_type);
	Socket(int socket_type, int fd);
	virtual ~Socket();
	int getSocketFd() const;
	int getSocketPort() const;
	std::string getSocketHost() const;
	int getSocketType() const;
};

#endif
