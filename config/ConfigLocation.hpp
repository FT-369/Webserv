#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP

#include "Utils.hpp"
class ConfigLocation
{
private:
	std::string url;
	short       url_prefix;
	std::map<std::string, std::string> simple_directive;
	/* data */
public:
	ConfigLocation(/* args */);
	ConfigLocation(std::string const &url);
	~ConfigLocation();
	int parsingLocation(std::string const &block);
	std::string getUrl();
	std::map<std::string, std::string> getSimpleDirective();
};

#endif