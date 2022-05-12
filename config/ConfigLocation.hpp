#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP

#include "configUtils.hpp"

#define REDIRECT "return"

class ConfigLocation
{
private:
	std::string _url;
	CommonDirective _common_directive;
	std::string _return_code;
	std::string _return_data;
	std::map<std::string, std::string> _simple_directive;

public:
	ConfigLocation(std::string const &url, CommonDirective const &c, std::string const &return_code, std::string const &return_data);
	~ConfigLocation();
	std::string getUrl();
	CommonDirective getCommonDirective();
	std::string getReturnCode();
	std::string getReturnData();
	std::map<std::string, std::string> getSimpleDirective();
	int parsingLocation(std::string const &block);

private:
	ConfigLocation();
	int parseLocationDirecive(std::map<std::string, std::string> &simple);
	int identifyBlock(std::string const &block);
};

#endif
