#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP

#include "ConfigLocation.hpp"

#define LISTEN "listen"
#define SERVER_NAME "server_name"

class ConfigServer
{
private:
	CommonDirective _common_directive;
	int _listen_port;
	std::string _listen_host;
	std::vector<std::string> _server_name;
	std::vector<ConfigLocation> _locations;
	std::map<std::string, std::string> _simple_directive;

public:
	ConfigServer(CommonDirective const &c);
	~ConfigServer();
	CommonDirective getCommonDirective() const;
	int getListenPort() const;
	std::string getListenHost() const;
	std::vector<std::string> getServerName() const;
	std::vector<ConfigLocation> getLocations() const;
	std::map<std::string, std::string> getSimpleDirective() const;
	void parsingServer(std::string const &block);

private:
	ConfigServer();
	void parseServerDirecive(std::map<std::string, std::string> &simple);
	void identifyLocationBlock(std::string const &block);
};

#endif
