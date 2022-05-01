#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation() { }

ConfigLocation::~ConfigLocation() { }

ConfigLocation::ConfigLocation(std::string const &url, CommonDirective const &c)
	: url(url), common_directive(c), return_code(-1) { }

std::string ConfigLocation::getUrl() { return this->url; }
CommonDirective ConfigLocation::getCommonDirective() { return common_directive; }
std::vector<std::string> ConfigLocation::getLimitExcept() { return limit_except; }
int ConfigLocation::getReturnCode() { return return_code; }
std::string ConfigLocation::getReturnDate() { return return_data; }
std::map<std::string, std::string> ConfigLocation::getSimpleDirective() { return simple_directive; }

int ConfigLocation::identifyBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);

	if (block_name != "") {
		// return ERROR;
	}
	return SUCCESS;
}

int ConfigLocation::parseLocationDirecive(std::map<std::string, std::string> &simple) {

	if (simple.find(REDIRECT) != simple.end()) {
		std::vector<std::string> redirect = ft_split_space(simple[REDIRECT]);
		if (redirect.size() < 1 || redirect.size() > 2)
			return ERROR;
		
		char *endptr = 0;
		double port = std::strtod(redirect[0].c_str(), &endptr);
		if (std::string(endptr) != "" || port != static_cast<int>(port) || port < 0)
			return ERROR;
		return_code = static_cast<int>(port);

		if (redirect.size() == 2) {
			return_data = redirect[1];
		}
	}

	if (simple.find(ALLOWED_METHOD) != simple.end()) {
		std::vector<std::string> method = ft_split_space(simple[ALLOWED_METHOD]);
		if (method.size() < 1 || method.size() > 3)
			return ERROR;
		
		for (size_t i = 0; i < method.size(); i++) {
			limit_except.push_back(method[i]);
		}
	}
	return SUCCESS;
}


int ConfigLocation::parsingLocation(std::string const &block) {
	size_t pos;
	std::string modify_block;
	std::vector<std::string> blocks;

	modify_block = sperateBrace(block);
	if ((pos = modify_block.find(MAIN_SEPARATOR)) != std::string::npos) {
		parseSimpleDirective(simple_directive, common_directive, block.substr(0, pos));
		parseLocationDirecive(simple_directive);
		blocks = ft_split(modify_block.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	} else {
		blocks = ft_split(modify_block, BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		identifyBlock(blocks[i]);
	}
	return SUCCESS;
}