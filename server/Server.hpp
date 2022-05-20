#ifndef SERVER_HPP
#define SERVER_HPP

#include "KqueueHandler.hpp"
#include "Socket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Request.hpp"
#include "Resource.hpp"
#include "webserv.hpp"

class Server
{
private:
	KqueueHandler _kq;
	std::map<uintptr_t, Socket *> _socket;
	Config _config;
public:
	Server();
	Server(Config &config);
	~Server();
	void serverConnect();
	KqueueHandler &getKqHandler();
	void setKqHandler(const KqueueHandler &kq);
	
	void acceptGetClientFd(ServerSocket *server_socket);
	void keventProcess();
	
	void setConfig(Config config);
	Config getConfig();
};

#endif
