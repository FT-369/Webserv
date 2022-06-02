#include "ConfigHttp.hpp"

ConfigHttp::ConfigHttp()
{
}

ConfigHttp::~ConfigHttp() {}

CommonDirective ConfigHttp::getCommonDirective() const { return _common_directive; }
std::vector<ConfigServer> &ConfigHttp::getServers() { return _servers; }
const std::vector<ConfigServer> &ConfigHttp::getServers() const { return _servers; }
std::map<std::string, std::string> ConfigHttp::getSimpleDirective() const { return _simple_directive; }

void ConfigHttp::identifyServerBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);

	if (block_name == "server")
	{
		ConfigServer server(_common_directive);
		server.parsingServer(block_content);
		if (!_simple_directive.empty())
			throw config_error("Invalid simple directive");	// 유효하지 않은 지시어가 남아있으면 에러
		_servers.push_back(server);
	}
	else if (!(block_name == "" && block_content == ""))
	{
		throw config_error("Invalid block directive");
	}
}

bool compareConfigServer(const ConfigServer &o1, const ConfigServer &o2) {
	return o1.getListenPort() > o2.getListenPort();
}

void ConfigHttp::parsingHttp(std::string const &block)
{
	size_t pos;
	std::string modify_block;
	std::vector<std::string> blocks;

	modify_block = sperateBrace(block);
	if ((pos = modify_block.find(MAIN_SEPARATOR)) != std::string::npos)
	{
		parseSimpleDirective(_simple_directive, _common_directive, block.substr(0, pos));
		blocks = ft_split(modify_block.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	}
	else
	{
		blocks = ft_split(modify_block, BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++)
	{
		identifyServerBlock(blocks[i]);
	}

	sort(_servers.begin(), _servers.end(), compareConfigServer);
	for (size_t i = 1; i < _servers.size(); i++)
	{
		if (_servers[i].getListenPort() == _servers[i - 1].getListenPort())
			throw config_error("Duplicate Port");
	}
}
