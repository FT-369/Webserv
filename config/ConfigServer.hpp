#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP

#include "ConfigLocation.hpp"

class ConfigServer
{
private:
	std::map<std::string, std::string> simple_directive;
	std::vector<ConfigLocation> locations;

public:
	ConfigServer();
	~ConfigServer();
	int parsingServer(std::string const &block);
	std::map<std::string, std::string> getSimpleDirective();
	std::vector<ConfigLocation> getLocations();

private:
	int identifyBlock(std::string const &block);
};

#endif
