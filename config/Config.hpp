#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigHttp.hpp"
#include <fstream>

#define PATH "../setting/"
#define DEFALUT_CONF "./setting/default.conf"
#define MIME_TYPES "./setting/mime.types"

class Config {
private:
	std::string filename;
	std::string	config_text;
	std::string default_type;
	std::map<std::string, std::string> mime_types;
	std::map<std::string, std::string> general_directive;
	ConfigHttp http_directive;
	std::map<int, std::string> status_code;

public:
	Config();
	Config(std::string const &filename);
	~Config();
	std::string getFileName();
	std::string getConfigText();
	std::map<std::string, std::string> getMimeTypes();
	std::map<std::string, std::string> getGeneralDirective();
	ConfigHttp getHttpDirective();

private: // func
	int identifyHttpBlock(std::string const &block);
	int parseGeneralDirective(std::map<std::string, std::string> &directive, std::string const &buffer);
	int checkBrace(std::stack<bool> &check_brace, std::string &buffer);
	void cutComment(std::string &buffer);
	int readConfigFile();
	int parsingConfig();
	int parsingMimeTypes(std::string const &filename);
	void setStatusCode();
};

#endif
