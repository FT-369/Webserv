#include "ConfigHttp.hpp"

ConfigHttp::ConfigHttp()
{
	// init default
}

ConfigHttp::~ConfigHttp() { }

std::map<std::string, std::string> ConfigHttp::getSimpleDirective() { return simple_directive;}
std::vector<ConfigServer> ConfigHttp::getServers() { return servers;}

int ConfigHttp::identifyBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);

	if (block_name == "server") {
		ConfigServer server;
		server.parsingServer(block_content);
		servers.push_back(server);
	} else if (block_name == "types") {
		parseSimpleDirective(this->mime_types, block_content);
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
		parseSimpleDirective(this->simple_directive, block.substr(0, pos));
		blocks = ft_split(modify_block.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	} else {
		blocks = ft_split(modify_block, BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		this->identifyBlock(blocks[i]);
	}
	return SUCCESS;
}