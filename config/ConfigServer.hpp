#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP

#include "ConfigLocation.hpp"

#define LISTEN "listen"
#define SERVER_NAME "server_name"

class ConfigServer
{
private:
	CommonDirective common_directive;
	int listen_port;
	std::string listen_host;
	std::string server_name;
	std::vector<ConfigLocation> locations;
	std::map<std::string, std::string> simple_directive;

public:
	ConfigServer(CommonDirective const &c);
	~ConfigServer();
	CommonDirective getCommonDirective();
	std::vector<ConfigLocation> getLocations();
	std::map<std::string, std::string> getSimpleDirective();
	int parsingServer(std::string const &block);

private:
	ConfigServer();
	int parseServerDirecive(std::map<std::string, std::string> &simple);
	int identifyLocationBlock(std::string const &block);
};

#endif
