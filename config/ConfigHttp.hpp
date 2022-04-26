#ifndef CONFIGHTTP_HPP
#define CONFIGHTTP_HPP

#include "ConfigServer.hpp"

class ConfigHttp
{
private:
	std::map<std::string, std::string> simple_directive;
	std::vector<ConfigServer> servers;

public:
	ConfigHttp(/* args */);
	~ConfigHttp();
};


#endif