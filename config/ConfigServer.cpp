#include "ConfigServer.hpp"

ConfigServer::ConfigServer()
{
	// init default
}

ConfigServer::~ConfigServer() { }

std::map<std::string, std::string> ConfigServer::getSimpleDirective() { return simple_directive; }
std::vector<ConfigLocation> ConfigServer::getLocations() { return locations; }

int ConfigServer::identifyBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);
	std::vector<std::string> block_name_arg = ft_split_space(block_name);

	if (block_name_arg.size() >= 2 && block_name_arg[0] == "location") {
		std::string url = block_name_arg[1];
		for (int i = 2; i < block_name_arg.size(); i++)
			url += " " + block_name_arg[i];
		
		ConfigLocation location(url);
		location.parsingLocation(block_content);
		locations.push_back(location);
	} else if (block_name == "types") {
		//
	} else {
		// return ERROR;
	}
	return SUCCESS;
}

int ConfigServer::parsingServer(std::string const &block) {
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