#ifndef SERVER_HPP
#define SERVER_HPP

#include "KqueueHandler.hpp"
#include "SocketController.hpp"


class Server
{
private :
	KqueueHandler kq;
	std::map<uintptr_t, Socket> socket;
	Config config;

private :
	Server();

public:
	Server(Config &config);
	~Server();
	void serverConnect();
	void acceptGetClientFd(ServerSocket *server_socket);
	void keventProcess();
	ServerSocket *Server::isServerFd(uintptr_t fd);
};

#endif
