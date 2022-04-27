#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation(/* args */)
{
	// init default;
}

ConfigLocation::~ConfigLocation() { }

ConfigLocation::ConfigLocation(std::string const &url) : url(url) { }

std::string ConfigLocation::getUrl() { return this->url; }
std::map<std::string, std::string> ConfigLocation::getSimpleDirective() { return this->simple_directive; }


int ConfigLocation::parsingLocation(std::string const &block) {
	size_t pos;
	std::string modify_block;
	std::vector<std::string> blocks;

	modify_block = sperateBrace(block);
	if ((pos = modify_block.find(MAIN_SEPARATOR)) != std::string::npos) {
		parseSimpleDirective(simple_directive, block.substr(0, pos));
	} else {
		// return ERROR;
	}
	return SUCCESS;
}