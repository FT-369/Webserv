#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP
#include "Socket.hpp"

class ServerSocket : public Socket
{
public:
	struct sockaddr_in _addr;
	ConfigServer _server_info;
	// std::vector<ConfigLocation> location;

private:
	ServerSocket();

public:
	ServerSocket(ConfigServer server);
	~ServerSocket();
	int binding();
	int clientAccept(int &connectFD);
	ConfigServer getServerInfo();
};

#endif
