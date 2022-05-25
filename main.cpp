#include "Config.hpp"
#include "Server.hpp"

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
	std::map<std::string, std::string>::iterator i_s_it;
	std::map<std::string, std::string>::iterator s_s_it;

	std::cout << C_AQUA << "Start of [" << name << "]\n" << C_NRML;
	
	std::cout << "root: " << common._root << std::endl;
	std::cout << "autoindex: " << (common._autoindex ? "true" : "false") << std::endl;
	std::cout << "client_limit_body_size: " << common._client_limit_body_size << std::endl;
	std::cout << "request_limit_header_size: " << common._request_limit_header_size << std::endl;
	for (size_t i = 0; i < common._index.size(); i++) {
		std::cout << "index[" << i + 1 << "]: " << common._index[i] << std::endl;
	}
	for (i_s_it = common._error_page.begin(); i_s_it != common._error_page.end(); i_s_it++) {
		std::cout << "error[" << i_s_it->first << "]: " << i_s_it->second << std::endl;
	}
	for (s_s_it = common._cgi_path.begin(); s_s_it != common._cgi_path.end(); s_s_it++) {
		std::cout << "cgi[" << s_s_it->first << "]: " << s_s_it->second << std::endl;
	}
	std::cout << "allowed Method: ";
	for (int i = 0; i < common._limit_except.size(); i++)
	{
		std::cout << common._limit_except[i] << (i + 1 == common._limit_except.size() ? "\n" : ", ");
	}
	std::cout << C_GREN << "End of [" << name << "]\n\n" << C_NRML;
}

void printServerDirective(std::string name, ConfigServer server) {
	

	std::cout << C_AQUA << "Start of [" << name << "]\n" << C_NRML;
	std::cout << "listen_port: " << server.getListenPort() << std::endl;
	std::cout << "listen_host: " << server.getListenHost() << std::endl;
	std::cout << "sever_name: ";
	for (size_t i = 0; i < server.getServerName().size(); i++) {
		std::cout << server.getServerName()[i] << (i + 1 == server.getServerName().size() ? "" : ", ");
	}
	std::cout << std::endl;
	std::cout << C_GREN << "End of [" << name << "]\n\n" << C_NRML;
}

void printLocationDirective(std::string name, ConfigLocation location) {
	

	std::cout << C_AQUA << "Start of [" << name << "]\n" << C_NRML;
	std::cout << "url : " << location.getUrl() << std::endl;
	std::cout << "return_code: " << location.getReturnCode() << std::endl;
	std::cout << "return_data: " << location.getReturnData() << std::endl;
	std::cout << std::endl;
	std::cout << C_GREN << "End of [" << name << "]\n\n" << C_NRML;
}

int main(void)
{
	Config config;

	try
	{
		config.setting();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 0;
	}
	
	Server server = Server(config);

	// std::cout << config.getConfigText() << std::endl;
	// std::map<std::string, std::string> main = config.getGeneralDirective();
	// ConfigHttp http = config.getHttpDirective();

	// printMap("general", main);
	// printMap("http_simple", http.getSimpleDirective());

	// std::vector<ConfigServer> servers = http.getServers();
	// for (int i = 0; i < servers.size(); i++) {
	// 	ConfigServer server = servers[i];
	// 	printCommonDirective("server_common_" + std::to_string(i + 1), server.getCommonDirective());
	// 	printMap("server_simple_" + std::to_string(i + 1), server.getSimpleDirective());
	// 	printServerDirective("server_directive_" + std::to_string(i + 1), server);
		
	// 	std::vector<ConfigLocation> locations = server.getLocations();
	// 	for (int j = 0; j < locations.size(); j++) {
	// 		ConfigLocation location = locations[j];
	// 		std::cout << C_YLLW << "location_url_" + std::to_string(i + 1) + "_" + std::to_string(j + 1) + ": " << C_NRML << location.getUrl() << std::endl;
	// 		printCommonDirective("location_common_" + std::to_string(i + 1) + "_" + std::to_string(j + 1), location.getCommonDirective());
	// 		printMap("location_simple_" + std::to_string(i + 1) + "_" + std::to_string(j + 1), location.getSimpleDirective());
	// 		printLocationDirective("location_" + std::to_string(i + 1), location);
	// 	}
	// }
	try
	{
		server.keventProcess();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return 0;
}
