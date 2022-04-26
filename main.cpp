#include "Config.hpp"
#include "Server.hpp"

int main(int argc, char const *argv[])
{
	Config *config;
	if (argc > 1){
		config = new Config(argv[1]);
	} else {
		config = new Config();
	}
	Server *server = new Server(*config);


	return 0;
}
