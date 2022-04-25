#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigHttp.hpp"
#include <fstream>
#include <stack>

class Config{
	private:
		std::map<std::string, std::string> main_directive;
		std::map<std::string, std::string> event_directive;
		ConfigHttp http_directive;
		const std::string filename;
		std::string	config_text;
		// int fd;
	
	public:
		Config();
		Config(const std::string &_filename);
		void printConfig();
		int identifyBlock();

	private: // func
		int checkBrace(std::stack<bool> &check_brace, const std::string &buffer, bool &first_brace);
		void cutComment(std::string &buffer);
		int readConfigFile();
		int parsingConfig();
		int findMain();
		void parseMainDirective(const std::string &buffer);

};

#endif