#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation() {}
ConfigLocation::~ConfigLocation() {}

ConfigLocation::ConfigLocation(std::string const &url, CommonDirective const &c)
	: _url(url), _common_directive(c), _return_code(-1) {}

std::string ConfigLocation::getUrl() { return _url; }
CommonDirective ConfigLocation::getCommonDirective() { return _common_directive; }
int ConfigLocation::getReturnCode() { return _return_code; }
std::string ConfigLocation::getReturnDate() { return _return_data; }
std::map<std::string, std::string> ConfigLocation::getSimpleDirective() { return _simple_directive; }

int ConfigLocation::identifyBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);

	if (block_name == "" && block_content == "")
	{
		return SUCCESS;
	}
	return ERROR; // location 블록 안에 또 다른 블록이 들어온 경우
}

int ConfigLocation::parseLocationDirecive(std::map<std::string, std::string> &simple)
{

	if (simple.find(REDIRECT) != simple.end())
	{
		std::vector<std::string> redirect = ft_split_space(simple[REDIRECT]);
		if (redirect.size() < 1 || redirect.size() > 2)
			return ERROR; // 지시어 형식이 맞지 않음

		char *endptr = 0;
		double d_port = std::strtod(redirect[0].c_str(), &endptr);
		int port = static_cast<int>(d_port);

		if (strlen(endptr) != 0 || d_port != port || d_port < 0)
			return ERROR; // !!! 올바른 value가 아님

		_return_code = port;
		if (redirect.size() == 2)
		{
			_return_data = redirect[1];
		}
		simple.erase(REDIRECT);
	}

	return SUCCESS;
}

int ConfigLocation::parsingLocation(std::string const &block)
{
	size_t pos;
	std::string modify_block;
	std::vector<std::string> blocks;

	modify_block = sperateBrace(block);
	if ((pos = modify_block.find(MAIN_SEPARATOR)) != std::string::npos)
	{
		parseSimpleDirective(_simple_directive, _common_directive, block.substr(0, pos));
		parseLocationDirecive(_simple_directive);
		blocks = ft_split(modify_block.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	}
	else
	{
		blocks = ft_split(modify_block, BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++)
	{
		identifyBlock(blocks[i]);
	}

	// if (!simple_directive.empty())
	// 	return ERROR;	// 유효하지 않은 지시어가 남아있으면 에러
	return SUCCESS;
}
