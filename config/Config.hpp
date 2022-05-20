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

private: // func
	int identifyHttpBlock(std::string const &block);
	int parseGeneralDirective(std::map<std::string, std::string> &directive, std::string const &buffer);
	int checkBrace(std::stack<bool> &check_brace, std::string &buffer);
	void cutComment(std::string &buffer);
	int readConfigFile();
	int parsingConfig();
};

#endif
