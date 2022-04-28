#ifndef CONFIGHTTP_HPP
#define CONFIGHTTP_HPP

#include "ConfigServer.hpp"

class ConfigHttp
{
private:
	std::map<std::string, std::string> simple_directive;
	std::vector<ConfigServer> servers;

public:
	ConfigHttp();
	~ConfigHttp();
	int parsingHttp(std::string const &block);
	std::map<std::string, std::string> getSimpleDirective();
	std::vector<ConfigServer> getServers();

private:
	int identifyBlock(std::string const &block);
};


#endif