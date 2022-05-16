#include "Socket.hpp"

Socket::Socket() {}

Socket::Socket(int socket_type) : _socket_type(socket_type) {}
Socket::Socket(int socket_type, int fd) : _socket_type(socket_type), _socket_fd(fd) {}

Socket::~Socket()
{
	// close(socket_fd);
}

int Socket::getSocketFd() const { return _socket_fd; }
int Socket::getSocketPort() const { return _port; }
std::string Socket::getSocketHost() const { return _host; }
int Socket::getSocketType() const { return _socket_type; }
