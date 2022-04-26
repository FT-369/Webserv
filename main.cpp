#include "Config.hpp"

int main(void)
{
	Config config;

	config.parsingConfig(DEFALUT_CONF);
	config.printConfig();

	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string> main = config.getMainDirective();
	std::map<std::string, std::string> event = config.getEventDirective();


	for (it = main.begin(); it != main.end(); it++)
	{
		std::cout << it->first << ": " << it->second << "|" << std::endl;
	}

	std::cout << "\n\n";
	
	for (it = event.begin(); it != event.end(); it++)
	{
		std::cout << it->first << ": " << it->second << "|" << std::endl;
	}
	
}