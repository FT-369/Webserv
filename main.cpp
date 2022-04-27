#include "Config.hpp"

# define C_NRML "\033[0m"
# define C_BLCK "\033[30m"
# define C_RED  "\033[31m"
# define C_GREN "\033[32m"
# define C_YLLW "\033[33m"
# define C_BLUE "\033[34m"
# define C_PRPL "\033[35m"
# define C_AQUA "\033[36m"


void printMap(std::string name, std::map<std::string, std::string> map) {
	std::map<std::string, std::string>::iterator it;

	std::cout << C_AQUA << "Start of [" << name << "]\n" << C_NRML;
	for (it = map.begin(); it != map.end(); it++)
	{
		std::cout << "{" << it->first << ": " << it->second << "}" << std::endl;
	}
	std::cout << C_GREN << "End of [" << name << "]\n\n" << C_NRML;
}

int main(void)
{
	Config config;

	config.parsingConfig(DEFALUT_CONF);
	// std::cout << config.getConfigText() << std::endl;

	std::map<std::string, std::string> main = config.getMainDirective();
	std::map<std::string, std::string> event = config.getEventDirective();
	ConfigHttp http = config.getHttpDirective();

	printMap("main", main);
	printMap("event", event);
	printMap("http_simple", http.getSimpleDirective());

	std::vector<ConfigServer> servers = http.getServers();
	for (int i = 0; i < servers.size(); i++) {
		ConfigServer server = servers[i];
		printMap("server_simple_" + std::to_string(i + 1), server.getSimpleDirective());
		
		std::vector<ConfigLocation> locations = server.getLocations();
		for (int j = 0; j < locations.size(); j++) {
			ConfigLocation location = locations[j];
			std::cout << C_YLLW << "location_url_" + std::to_string(i + 1) + "_" + std::to_string(j + 1) + ": " << C_NRML << location.getUrl() << std::endl;
			printMap("location_simple_" + std::to_string(i + 1) + "_" + std::to_string(j + 1), location.getSimpleDirective());
		}
	}
	
}