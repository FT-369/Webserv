#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigHttp.hpp"
#include <fstream>

#define PATH "../setting/"
#define DEFALUT_CONF "./setting/default.conf"

class Config {
	private:
		std::string filename;
		std::string	config_text;
		std::map<std::string, std::string> main_directive;
		std::map<std::string, std::string> event_directive;
		ConfigHttp http_directive;
	
	public:
		Config();
		~Config();
		std::string getFileName();
		std::string getConfigText();
		std::map<std::string, std::string> getMainDirective();
		std::map<std::string, std::string> getEventDirective();
		ConfigHttp getHttpDirective();
		int parsingConfig(std::string const &filename);

	private: // func
		void cutComment(std::string &buffer);
		int checkBrace(std::stack<bool> &check_brace, std::string &buffer);
		int readConfigFile();
		int identifyBlock(std::string const &block);
};

#endif
