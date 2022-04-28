#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation()
{
	// init default;
}

ConfigLocation::~ConfigLocation() { }

ConfigLocation::ConfigLocation(std::string const &url) : url(url) { }

std::string ConfigLocation::getUrl() { return this->url; }
std::map<std::string, std::string> ConfigLocation::getSimpleDirective() { return this->simple_directive; }

int ConfigLocation::identifyBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);
	std::vector<std::string> block_name_arg = ft_split_space(block_name);

	if (block_name == "types") {
		parseSimpleDirective(this->mime_types, block_content);
	} else if (block_name != "") {
		// return ERROR;
	}
	return SUCCESS;
}

int ConfigLocation::parsingLocation(std::string const &block) {
	size_t pos;
	std::string modify_block;
	std::vector<std::string> blocks;

	modify_block = sperateBrace(block);
	if ((pos = modify_block.find(MAIN_SEPARATOR)) != std::string::npos) {
		parseSimpleDirective(simple_directive, block.substr(0, pos));
		blocks = ft_split(modify_block.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	} else {
		blocks = ft_split(modify_block, BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		this->identifyBlock(blocks[i]);
	}

	return SUCCESS;
}