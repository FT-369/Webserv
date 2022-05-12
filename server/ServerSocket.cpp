#include "ServerSocket.hpp"

ServerSocket::ServerSocket(ConfigServer server) : Socket(SERVER_SOCKET), _server_info(server)
{
	if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
	}
	// throw error
	_port = server.getListenPort();
	_host = server.getListenHost().c_str();
	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	// _addr.sin_addr.s_addr = inet_addr(host.c_str());
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(_port);
}

int ServerSocket::binding()
{
	// if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) //바로 주소 재사용가능하도록 설정
	// 	throw (setsockopt_error());
	// if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) == -1) //바로 포트 재사용가능하도록 설정
	// 	throw (setsockopt_error());
	if (bind(_socket_fd, reinterpret_cast<struct sockaddr *>(&_addr), sizeof(_addr)) == -1)
	{
		std::cout << "bind() error" << std::endl;
		return ERROR;
	}
	if (listen(_socket_fd, 20) == -1)
	{
		std::cout << "listen() error" << std::endl;
		return ERROR;
	}
	if (fcntl(_socket_fd, F_SETFL, O_NONBLOCK))
	{
		std::cout << "fcntl() error" << std::endl;
		return ERROR;
	}
	return SUCCESS;
}

int ServerSocket::clientAccept(int &connectFD)
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);

	connectFD = accept(_socket_fd, (struct sockaddr *)&client_addr, &client_addr_size);
	if (connectFD == -1)
	{
		std::cout << "server_socket: accept() error" << std::endl;
		return ERROR;
	}
	fcntl(connectFD, F_SETFL, O_NONBLOCK);
	return ERROR;
}

ServerSocket::~ServerSocket()
{
	close(_socket_fd);
}

ConfigServer ServerSocket::getServerInfo() { return _server_info; }
