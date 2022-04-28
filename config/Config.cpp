#include "Config.hpp"

Config::Config() : filename(DEFALUT_CONF), config_text("")
{
	// init default
}

Config::~Config() { }

std::string Config::getFileName() { return filename; }
std::string Config::getConfigText() { return config_text; }
std::map<std::string, std::string> Config::getMainDirective() { return main_directive; }
std::map<std::string, std::string> Config::getEventDirective() { return event_directive; }
ConfigHttp Config::getHttpDirective() { return http_directive; }

void Config::cutComment(std::string &buffer)
{
	size_t	pos = buffer.find("#");

	if (pos != std::string::npos) {
		buffer = buffer.substr(0, pos);
	}
}

int Config::checkBrace(std::stack<bool> &check_brace, std::string &buffer)
{
	std::string modify_buffer;

	for (int i = 0; i < buffer.length(); i++)
	{
		modify_buffer += buffer[i];
		if (buffer[i] == '{') {
			check_brace.push(1);
		}
		if (buffer[i] == '}') {
			if (check_brace.empty()) {
				config_text = "ERROR!";
				return ERROR;
			}
			check_brace.pop();
			if (check_brace.empty()) {
				modify_buffer += BLOCK_SEPARATOR;
			}
		}
	}
	buffer = modify_buffer;
	return SUCCESS;
}

int Config::readConfigFile()
{
	std::ifstream is(filename);
	std::string buffer;
	std::stack<bool> check_brace;
	bool first_brace = false;
	size_t pos;

	if (is.fail())
	{
		std::cerr << "Unable to find the file: " << filename << std::endl;
		return ERROR;
	}
	while (std::getline(is, buffer))
	{
		cutComment(buffer);
		if (checkBrace(check_brace, buffer)) {
			config_text = "ERROR!";
			return ERROR;
		}
		config_text += buffer;
		if (first_brace == false && (pos = config_text.find("{")) != std::string::npos) {
			first_brace = true;
			size_t rpos = config_text.rfind(';', pos);
			config_text.insert(rpos + 1, MAIN_SEPARATOR);
		}
	}
	if (!check_brace.empty()) {
		config_text = "ERROR!";
		return ERROR;
	}
	return SUCCESS;
}

int Config::identifyBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);

	if (block_name == "events") {
		parseSimpleDirective(this->event_directive, block_content);
	} else if (block_name == "types") {
		//
	} else if (block_name == "http") {
		http_directive.parsingHttp(block_content);
	} else if (block_name != "") {
		// return ERROR;
	}
	return SUCCESS;
}

int Config::parsingConfig(std::string const &filename)
{
	size_t pos;
	std::vector<std::string> blocks;

	this->filename = filename;
	if (readConfigFile() == ERROR)
		return ERROR;
	
	pos = config_text.find(MAIN_SEPARATOR);
	if (pos == std::string::npos) {
		blocks = ft_split(config_text, BLOCK_SEPARATOR);
	} else {
		parseSimpleDirective(main_directive, config_text.substr(0, pos));
		blocks = ft_split(config_text.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		this->identifyBlock(blocks[i]);
	}
	return SUCCESS;
}