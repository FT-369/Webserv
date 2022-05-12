#ifndef CONFIGHTTP_HPP
#define CONFIGHTTP_HPP

#include "ConfigServer.hpp"

class ConfigHttp
{
private:
	CommonDirective _common_directive;
	std::vector<ConfigServer> _servers;
	std::map<std::string, std::string> _simple_directive;

public:
	ConfigHttp();
	~ConfigHttp();
	CommonDirective getCommonDirective();
	std::vector<ConfigServer> &getServers();
	std::map<std::string, std::string> getSimpleDirective();
	int parsingHttp(std::string const &block);

private:
	int identifyServerBlock(std::string const &block);
};

#endif
