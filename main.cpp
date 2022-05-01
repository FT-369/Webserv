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

void printCommonDirective(std::string name, CommonDirective common) {
	std::map<int, std::string>::iterator i_s_it;
	std::map<std::string, std::string>::iterator s_s_it;

	std::cout << C_AQUA << "Start of [" << name << "]\n" << C_NRML;
	
	std::cout << "root: " << common.root << std::endl;
	std::cout << "autoindex: " << (common.autoindex ? "true" : "false") << std::endl;
	std::cout << "client_limit_body_size: " << common.client_limit_body_size << std::endl;
	std::cout << "request_limit_header_size: " << common.request_limit_header_size << std::endl;
	for (size_t i = 0; i < common.index.size(); i++) {
		std::cout << "index[" << i + 1 << "]: " << common.index[i] << std::endl;
	}
	for (i_s_it = common.error_page.begin(); i_s_it != common.error_page.end(); i_s_it++) {
		std::cout << "error[" << i_s_it->first << "]: " << i_s_it->second << std::endl;
	}
	for (s_s_it = common.cgi_path.begin(); s_s_it != common.cgi_path.end(); s_s_it++) {
		std::cout << "error[" << s_s_it->first << "]: " << s_s_it->second << std::endl;
	}
	std::cout << C_GREN << "End of [" << name << "]\n\n" << C_NRML;
}


int main(void)
{
	Config config;

	config.parsingConfig(DEFALUT_CONF);
	// std::cout << config.getConfigText() << std::endl;

	std::map<std::string, std::string> main = config.getGeneralDirective();
	ConfigHttp http = config.getHttpDirective();

	printMap("main", main);
	printMap("http_simple", http.getSimpleDirective());

	std::vector<ConfigServer> servers = http.getServers();
	for (int i = 0; i < servers.size(); i++) {
		ConfigServer server = servers[i];
		printCommonDirective("server_common_" + std::to_string(i + 1), server.getCommonDirective());
		printMap("server_simple_" + std::to_string(i + 1), server.getSimpleDirective());
		
		std::vector<ConfigLocation> locations = server.getLocations();
		for (int j = 0; j < locations.size(); j++) {
			ConfigLocation location = locations[j];
			std::cout << C_YLLW << "location_url_" + std::to_string(i + 1) + "_" + std::to_string(j + 1) + ": " << C_NRML << location.getUrl() << std::endl;
			printCommonDirective("location_common_" + std::to_string(i + 1) + "_" + std::to_string(j + 1), location.getCommonDirective());
			printMap("location_simple_" + std::to_string(i + 1) + "_" + std::to_string(j + 1), location.getSimpleDirective());
		}
	}
	
}