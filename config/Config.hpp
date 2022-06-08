#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigHttp.hpp"

#define DEFALUT_CONF "./setting/default.conf"

class Config
{
private:
	std::string _filename;
	std::string _config_text;
	std::map<std::string, std::string> _general_directive;
	ConfigHttp _http_directive;

public:
	Config();
	~Config();
	std::string getFileName() const;
	std::string getConfigText() const;
	std::map<std::string, std::string> getGeneralDirective() const;
	ConfigHttp getHttpDirective() const;
	void setting();
	void setting(std::string const &filename);

private:
	void identifyHttpBlock(std::string const &block);
	void parseGeneralDirective(std::map<std::string, std::string> &directive, std::string const &buffer);
	void checkBrace(std::stack<bool> &check_brace, std::string &buffer);
	void cutComment(std::string &buffer);
	void readConfigFile();
	void parsingConfig();
};

#endif
