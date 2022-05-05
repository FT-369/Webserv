#ifndef SERVER_HPP
#define SERVER_HPP

#include "KqueueHandler.hpp"
#include "Socket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"

class Server
{
private :
	KqueueHandler kq;
	std::map<uintptr_t, Socket *> socket;
	Config config;

private :
	Server();

public:
	Server(Config &config);
	~Server();
	void serverConnect();
	void acceptGetClientFd(ServerSocket *server_socket);
	void keventProcess();
	ServerSocket *isServerFd(uintptr_t fd);
};

#endif
