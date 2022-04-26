#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigHttp.hpp"
#include "Utils.hpp"
#include <fstream>
#include <stack>

#define PATH "../setting/"
#define DEFALUT_CONF "./setting/default.conf"
#define MAIN_SEPARATOR "\n\n"
#define BLOCK_SEPARATOR "\n"

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
		std::map<std::string, std::string> getMainDirective();
		std::map<std::string, std::string> getEventDirective();
		int parsingConfig(std::string const &filename);
		void printConfig();

	private: // func
		void cutComment(std::string &buffer);
		int checkBrace(std::stack<bool> &check_brace, std::string &buffer);
		int readConfigFile();
		std::string getBlockName(std::string const &block);
		int identifyBlock(std::string const &block);
};

#endif
