#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation() {}
ConfigLocation::~ConfigLocation() {}

ConfigLocation::ConfigLocation(std::string const &url, CommonDirective const &c, std::string const &return_code,
 std::string const &return_data)
	: _url(url), _common_directive(c), _return_code(return_code), _return_data(return_data) {}

std::string ConfigLocation::getUrl() const { return _url; }
CommonDirective ConfigLocation::getCommonDirective() const { return _common_directive; }
std::string ConfigLocation::getReturnCode() const { return _return_code; }
std::string ConfigLocation::getReturnData() const { return _return_data; }
std::map<std::string, std::string> ConfigLocation::getSimpleDirective() const { return _simple_directive; }

void ConfigLocation::identifyBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);

	if (!(block_name == "" && block_content == ""))
	{
		throw config_error("There is a block inside the locations block"); // location 블록 안에 또 다른 블록이 들어온 경우
	}
}

void ConfigLocation::parseLocationDirecive(std::map<std::string, std::string> &simple)
{

	if (simple.find(REDIRECT) != simple.end())
	{
		std::vector<std::string> redirect = ft_split_space(simple[REDIRECT]);
		if (redirect.size() < 1 || redirect.size() > 2)
			throw config_parsing_error("directive or value of directive does not exist"); // 지시어 형식이 맞지 않음

		char *endptr = 0;
		double d_port = std::strtod(redirect[0].c_str(), &endptr);
		int port = static_cast<int>(d_port);

		if (strlen(endptr) != 0 || d_port != port || d_port > 399 || d_port < 300)
			throw config_parsing_error("Invalid directive value"); // !!!올바른 value가 아님
		_return_code = redirect[0];
		if (redirect.size() == 2)
		{
			_return_data = redirect[1];
		}
		simple.erase(REDIRECT);
	}
}

void ConfigLocation::parsingLocation(std::string const &block)
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
}
