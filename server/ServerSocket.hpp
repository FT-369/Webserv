#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP
#include "Socket.hpp"
#include <cstring>

class ServerSocket : public Socket
{
public:
	struct sockaddr_in addr;
	ConfigServer server_info;
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
