#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char **argv)
{
	Config config;

	try
	{
		if (argc > 1)
			config.setting(std::string(argv[1]));
		else
			config.setting();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 0;
	}
	Server server = Server(config);
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
