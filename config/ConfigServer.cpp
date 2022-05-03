#include "ConfigServer.hpp"

ConfigServer::ConfigServer() { }

ConfigServer::ConfigServer(CommonDirective const &c) : common_directive(c), listen_port(-1) { }

ConfigServer::~ConfigServer() { }

CommonDirective  ConfigServer::getCommonDirective() { return common_directive; }
int ConfigServer::getListenPort() { return listen_port; }
std::string ConfigServer::getListenHost() { return listen_host; }
std::vector<std::string> ConfigServer::getServerName() { return server_name; }
std::vector<ConfigLocation> ConfigServer::getLocations() { return locations; }
std::map<std::string, std::string> ConfigServer::getSimpleDirective() { return simple_directive; }

int ConfigServer::identifyLocationBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);
	std::vector<std::string> block_name_arg = ft_split_space(block_name);

	if (block_name_arg.size() >= 2 && block_name_arg[0] == "location") {
		std::string url = block_name_arg[1];
		for (int i = 2; i < block_name_arg.size(); i++)
			url += " " + block_name_arg[i];
		
		ConfigLocation location(url, common_directive);
		location.parsingLocation(block_content);
		locations.push_back(location);
	} else if (!(block_name == "" && block_content == "")) {
		return ERROR;	// 유효하지 않은 블럭
	}
	return SUCCESS;
}

int ConfigServer::parseServerDirecive(std::map<std::string, std::string> &simple) {

	if (simple.find(LISTEN) != simple.end()) {
		std::vector<std::string> listen = ft_split_space(simple[LISTEN]);
		if (listen.size() < 1 || listen.size() > 2)
			return ERROR;
		
		char *endptr = 0;
		double d_port = std::strtod(listen[0].c_str(), &endptr);
		int port = static_cast<int>(d_port);
		if (std::string(endptr) != "" || d_port != port || d_port < 0)
			return ERROR;	// !!! 올바른 value가 아님
		listen_port = port;

		if (listen.size() == 2) {
			listen_host = listen[1];
		}
		simple.erase(LISTEN);
	}

	if (simple.find(SERVER_NAME) != simple.end()) {
		std::vector<std::string> names = ft_split_space(simple[SERVER_NAME]);
		if (names.empty())
			return ERROR;	// 지시어 형식이 맞지 않음
		server_name = names;
		simple.erase(SERVER_NAME);
	}
	return SUCCESS;
}

int ConfigServer::parsingServer(std::string const &block) {
	size_t pos;
	std::string modify_block;
	std::vector<std::string> blocks;

	modify_block = sperateBrace(block);
	if ((pos = modify_block.find(MAIN_SEPARATOR)) != std::string::npos) {
		parseSimpleDirective(simple_directive, common_directive, block.substr(0, pos));
		parseServerDirecive(simple_directive);
		blocks = ft_split(modify_block.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	} else {
		blocks = ft_split(modify_block, BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		identifyLocationBlock(blocks[i]);
	}

	// if (!simple_directive.empty())
	// 	return ERROR;	// 유효하지 않은 지시어가 남아있으면 에러
	return SUCCESS;
}