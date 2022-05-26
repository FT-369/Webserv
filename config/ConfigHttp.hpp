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
	CommonDirective getCommonDirective() const;
	std::vector<ConfigServer> &getServers();
	const std::vector<ConfigServer> &getServers() const;
	std::map<std::string, std::string> getSimpleDirective() const;
	void parsingHttp(std::string const &block);

private:
	void identifyServerBlock(std::string const &block);
};

#endif
