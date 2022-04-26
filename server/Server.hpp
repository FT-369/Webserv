#ifndef SERVER_HPP
#define SERVER_HPP
#include "Config.hpp"

class Server
{
private:
	Server();

public:
	Server(Config &config);
	~Server();
};

#endif
