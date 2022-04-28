#ifndef SERVER_HPP
#define SERVER_HPP
#include "Config.hpp"

class Server
{
private:
	Server();
	Config *config;

public:
	Server(Config &config);
	~Server();
};

#endif
