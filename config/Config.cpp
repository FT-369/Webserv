#include "Config.hpp"

Config::Config() : _filename(""), _config_text("")
{
	GlobalConfig::initMimeTyeps();
	GlobalConfig::initStatusCode();
}

Config::~Config() {}

std::string Config::getFileName() const { return _filename; }
std::string Config::getConfigText() const { return _config_text; }
std::map<std::string, std::string> Config::getGeneralDirective() const { return _general_directive; }
ConfigHttp Config::getHttpDirective() const { return _http_directive; }

void Config::cutComment(std::string &buffer)
{
	size_t pos = buffer.find("#");
	size_t pos2 = buffer.find("//");

	if (pos != std::string::npos)
	{
		buffer = buffer.substr(0, pos);
	}
	if (pos2 != std::string::npos)
	{
		buffer = buffer.substr(0, pos2);
	}
}

void Config::checkBrace(std::stack<bool> &check_brace, std::string &buffer)
{
	std::string modify_buffer;

	for (size_t i = 0; i < buffer.length(); i++)
	{
		modify_buffer += buffer[i];
		if (buffer[i] == '{')
		{
			check_brace.push(1);
		}
		if (buffer[i] == '}')
		{
			if (check_brace.empty())
				throw config_parsing_error("braces mismatch");
			
			check_brace.pop();
			if (check_brace.empty())
			{
				modify_buffer += BLOCK_SEPARATOR;
			}
		}
	}
	buffer = modify_buffer;
}

void Config::identifyHttpBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);

	if (block_name == "http")
	{
		_http_directive.parsingHttp(block_content);
	}
	else if (!(block_name == "" && block_content == ""))
	{
		throw config_error("Invalid block directive");
	}
}

void Config::parseGeneralDirective(std::map<std::string, std::string> &directive, std::string const &buffer)
{
	std::vector<std::string> main_line;
	std::vector<std::string> split_line;
	std::string value;

	main_line = ft_split(buffer, ";");
	for (size_t i = 0; i < main_line.size(); i++)
	{
		split_line = ft_split_space(main_line[i]);
		if (split_line.size() < 2)
			throw config_parsing_error("directive or value of directive does not exist"); // 지시어 형식이 맞지 않음
		value = split_line[1];
		for (size_t i = 2; i < split_line.size(); i++)
			value += " " + split_line[i];
		directive[split_line[0]] = value;
	}
}

void Config::readConfigFile()
{
	std::ifstream is(_filename);
	std::string buffer;
	std::stack<bool> check_brace;
	bool first_brace = false;
	size_t pos;

	if (is.fail())
		throw file_error("Failed to open file");

	while (std::getline(is, buffer))
	{
		cutComment(buffer);
		checkBrace(check_brace, buffer);
		_config_text += buffer;
		if (first_brace == false && (pos = _config_text.find("{")) != std::string::npos)
		{
			first_brace = true;
			size_t rpos = _config_text.rfind(';', pos);
			_config_text.insert(rpos + 1, MAIN_SEPARATOR);
		}
	}
	if (!check_brace.empty())
		throw config_parsing_error("braces mismatch");
}

void Config::parsingConfig()
{
	size_t pos;
	std::vector<std::string> blocks;

	readConfigFile();

	pos = _config_text.find(MAIN_SEPARATOR);
	if (pos == std::string::npos || pos == 0)
	{
		blocks = ft_split(_config_text, BLOCK_SEPARATOR);
	}
	else
	{
		parseGeneralDirective(_general_directive, _config_text.substr(0, pos));
		blocks = ft_split(_config_text.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++)
	{
		identifyHttpBlock(blocks[i]);
	}
}

void Config::setting()
{
	_filename = DEFALUT_CONF;
	if (!isFile(_filename))
		throw file_error("Unable to find the file '" + _filename + "'");
	parsingConfig();
}

void Config::setting(std::string const &filename)
{
	_filename = filename;
	if (!isFile(_filename))
		throw file_error("Unable to find the file '" + _filename + "'");
	parsingConfig();
}
