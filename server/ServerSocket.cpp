#include "ServerSocket.hpp"

ServerSocket::ServerSocket(ConfigServer server) : Socket(SERVER_SOCKET), _server_info(server)
{
	if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		throw socket_error("socket()");
	}
	_port = server.getListenPort();
	_host = server.getListenHost().c_str();
	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(_port);
}

void ServerSocket::binding()
{
	int reuse = 1;

	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
	{
		throw socket_error("setsockopt(SO_REUSEADDR)");
	}
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) == -1)
	{
		throw socket_error("setsockopt(SO_REUSEPORT)");
	}
	if (bind(_socket_fd, reinterpret_cast<struct sockaddr *>(&_addr), sizeof(_addr)) == -1)
	{
		throw socket_error("bind()");
	}
	if (listen(_socket_fd, 100000) == -1)
	{
		throw socket_error("listen()");
	}
	if (fcntl(_socket_fd, F_SETFL, O_NONBLOCK))
	{
		throw socket_error("server socket fcntl()");
	}
}

void ServerSocket::clientAccept(int &connectFD)
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);

	connectFD = accept(_socket_fd, (struct sockaddr *)&client_addr, &client_addr_size);
	if (connectFD == -1)
	{
		throw socket_error("accept()");
	}
	if (fcntl(connectFD, F_SETFL, O_NONBLOCK))
	{
		throw socket_error("client socket fcntl()");
	}

	struct timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	if (setsockopt(connectFD, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)) == -1)
		throw socket_error("setsockopt(TIMEOUT)");
	if (setsockopt(connectFD, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval)) == -1)
		throw socket_error("setsockopt(TIMEOUT)");
}

ServerSocket::~ServerSocket()
{
	close(_socket_fd);
}

ConfigServer ServerSocket::getServerInfo() { return _server_info; }
