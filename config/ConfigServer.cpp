#include "ConfigServer.hpp"

ConfigServer::ConfigServer() {}

ConfigServer::ConfigServer(CommonDirective const &c) : _common_directive(c), _listen_port(80), _listen_host("127.0.0.1") {}

ConfigServer::~ConfigServer() {}

CommonDirective ConfigServer::getCommonDirective() const { return _common_directive; }
int ConfigServer::getListenPort() const { return _listen_port; }
std::string ConfigServer::getListenHost() const { return _listen_host; }
std::vector<std::string> ConfigServer::getServerName() const { return _server_name; }
std::vector<ConfigLocation> ConfigServer::getLocations() const { return _locations; }
std::map<std::string, std::string> ConfigServer::getSimpleDirective() const { return _simple_directive; }

void ConfigServer::identifyLocationBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);
	std::vector<std::string> block_name_arg = ft_split_space(block_name);

	if (block_name_arg.size() >= 2 && block_name_arg[0] == "location")
	{
		std::string url = block_name_arg[1];
		for (size_t i = 2; i < block_name_arg.size(); i++)
			url += " " + block_name_arg[i];

		ConfigLocation location(url, _common_directive, "", "");
		location.parsingLocation(block_content);
		if (!_simple_directive.empty())
			throw config_error("Invalid simple directive");	// 유효하지 않은 지시어가 남아있으면 에러
		_locations.push_back(location);
	}
	else if (!(block_name == "" && block_content == ""))
	{
		throw config_error("Invalid block directive");
	}
}

void ConfigServer::parseServerDirecive(std::map<std::string, std::string> &simple)
{

	if (simple.find(LISTEN) != simple.end())
	{
		std::vector<std::string> listen = ft_split_space(simple[LISTEN]);
		if (listen.size() != 1)
			throw config_parsing_error("Invalid directive value");
		if (listen[0].find(":") != std::string::npos)
		{
			std::vector<std::string> host_port = ft_split(listen[0], ":");
			if (host_port.size() != 2)
				throw config_parsing_error("Invalid directive value");
			_listen_host = host_port[0];

			char *endptr = 0;
			double d_port = std::strtod(host_port[1].c_str(), &endptr);
			int port = static_cast<int>(d_port);
			if (std::string(endptr) != "" || d_port != port || d_port < 0)
				throw config_parsing_error("Invalid directive value"); // 지시어 형식이 맞지 않음
			_listen_port = port;
		}
		else
		{
			bool isNumber = true;
			for (size_t i = 0; i < listen[0].size(); i++)
			{
				if (std::isdigit(listen[0][i]) == 0)
					isNumber = false;
			}
			if (isNumber)
			{
				char *endptr = 0;
				double d_port = std::strtod(listen[0].c_str(), &endptr);
				int port = static_cast<int>(d_port);
				if (std::string(endptr) != "" || d_port != port || d_port < 0)
					throw config_parsing_error("Invalid directive value"); // 지시어 형식이 맞지 않음
				_listen_port = port;
			}
			else
			{
				_listen_host = listen[0];
			}
		}
		simple.erase(LISTEN);
	}

	if (simple.find(SERVER_NAME) != simple.end())
	{
		std::vector<std::string> names = ft_split_space(simple[SERVER_NAME]);
		if (names.empty())
			throw config_parsing_error("value of directive does not exist"); // 지시어 형식이 맞지 않음
		_server_name = names;
		simple.erase(SERVER_NAME);
	}
}

void ConfigServer::parsingServer(std::string const &block)
{
	size_t pos;
	std::string modify_block;
	std::vector<std::string> blocks;

	modify_block = sperateBrace(block);
	if ((pos = modify_block.find(MAIN_SEPARATOR)) != std::string::npos)
	{
		parseSimpleDirective(_simple_directive, _common_directive, block.substr(0, pos));
		parseServerDirecive(_simple_directive);
		blocks = ft_split(modify_block.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	}
	else
	{
		blocks = ft_split(modify_block, BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++)
	{
		identifyLocationBlock(blocks[i]);
	}
}
