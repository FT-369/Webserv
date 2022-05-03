#include "ServerSocket.hpp"

ServerSocket::ServerSocket(ConfigServer server) : Socket(SERVER_SOCKET)
{
	if (socket_fd = socket(AF_INET, SOCK_STREAM, 0) == -1);
		// throw error
	port = server.getListenPort();
	host = server.getListenHost().c_str();
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(host.c_str());
	addr.sin_port = htons(port);
}

int ServerSocket::binding()
{
	// if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) //바로 주소 재사용가능하도록 설정
	// 	throw (setsockopt_error());
	// if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) == -1) //바로 포트 재사용가능하도록 설정
	// 	throw (setsockopt_error());
	if (bind(socket_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
	{
		std::cout << "bind() error" << std::endl;
		return ERROR;
	}
	if (listen(socket_fd, 20) == -1)
	{
		std::cout << "listen() error" << std::endl;
		return ERROR;
	}
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK))
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

	int connectFD = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_size);
	if (connectFD == -1) {
		std::cout << "server_socket: accept() error" << std::endl;
		return ERROR;
	}
	fcntl(connectFD, F_SETFL, O_NONBLOCK);
	return connectFD;
}

ServerSocket::~ServerSocket()
{
	close(socket_fd);
}
