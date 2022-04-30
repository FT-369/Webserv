#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP

#include "configUtils.hpp"

class ConfigLocation
{
private:
	std::string url;
	short url_prefix;
	std::map<std::string, std::string> simple_directive;
	std::map<std::string, std::string> mime_types;

public:
	ConfigLocation(std::string const &url);
	~ConfigLocation();
	int parsingLocation(std::string const &block);
	std::string getUrl();
	std::map<std::string, std::string> getSimpleDirective();
	std::map<std::string, std::string> getMimeTypes();
	
private:
	ConfigLocation();
	int identifyBlock(std::string const &block);
	// url parsing
};

#endif