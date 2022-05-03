#include "ConfigHttp.hpp"

ConfigHttp::ConfigHttp()
{
	// init default
}

ConfigHttp::~ConfigHttp() { }

CommonDirective  ConfigHttp::getCommonDirective() { return common_directive; }
std::vector<ConfigServer> &ConfigHttp::getServers() { return servers; }
std::map<std::string, std::string> ConfigHttp::getSimpleDirective() { return simple_directive; }

int ConfigHttp::identifyServerBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);

	if (block_name == "server") {
		ConfigServer server(common_directive);
		server.parsingServer(block_content);
		servers.push_back(server);
	} else if (block_name != "") {
		// return ERROR;
	}
	return SUCCESS;
}

int ConfigHttp::parsingHttp(std::string const &block) {
	size_t pos;
	std::string modify_block;
	std::vector<std::string> blocks;

	modify_block = sperateBrace(block);
	if ((pos = modify_block.find(MAIN_SEPARATOR)) != std::string::npos) {
		parseSimpleDirective(simple_directive, common_directive, block.substr(0, pos));
		blocks = ft_split(modify_block.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	} else {
		blocks = ft_split(modify_block, BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		identifyServerBlock(blocks[i]);
	}
	return SUCCESS;
}
