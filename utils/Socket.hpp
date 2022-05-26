#ifndef SOCKET_HPP
#define SOCKET_HPP
#include "ConfigServer.hpp"
#include "webserv.hpp"

#define SERVER_SOCKET 1
#define CLIENT_SOCKET 2
#define RESOURCE_FD 3
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
	void setSocketType(int socket_type);
	int getSocketFd() const;
	int getSocketPort() const;
	std::string getSocketHost() const;
	int getSocketType() const;
};

#endif
