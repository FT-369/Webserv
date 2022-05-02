#include "Socket.hpp"

Socket::Socket() { }

Socket::~Socket()
{
	// close(socket_fd);
}

int Socket::getSocketFd() { return socket_fd; }
int Socket::getSocketPort() { return port; }
std::string Socket::getSocketHost() { return host; }
int Socket::getSocketType() { return socket_type; }
