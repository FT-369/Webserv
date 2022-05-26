#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Socket.hpp"
#include "webserv.hpp"

class ServerSocket : public Socket
{
public:
	struct sockaddr_in _addr;
	ConfigServer _server_info;

private:
	ServerSocket();

public:
	ServerSocket(ConfigServer server);
	~ServerSocket();
	void binding();
	void clientAccept(int &connectFD);
	ConfigServer getServerInfo();
};

#endif
