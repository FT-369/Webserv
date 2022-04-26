#include "Config.hpp"

Config::Config() : filename(DEFALUT_CONF), config_text("") {
	// init default
}

Config::~Config() { }

std::string Config::getFileName() { return filename; }
std::map<std::string, std::string> Config::getMainDirective() { return main_directive; }
std::map<std::string, std::string> Config::getEventDirective() { return event_directive; }

void Config::cutComment(std::string &buffer) {
	size_t	pos = buffer.find("#");

	if (pos != std::string::npos) {
		buffer = buffer.substr(0, pos);
	}
}

int Config::checkBrace(std::stack<bool> &check_brace, std::string &buffer) {
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
			size_t rpos = config_text.rfind(';');
			config_text.insert(rpos + 1, MAIN_SEPARATOR);
		}
	}
	if (!check_brace.empty()) {
		config_text = "ERROR!";
		return ERROR;
	}
	return SUCCESS;
}

std::string Config::getBlockName(std::string const &block)
{
	size_t pos = block.find("{");

	return ft_trim(block.substr(0, pos));
}

int Config::identifyBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);

	if (block_name == "events") {
		size_t open_pos = block.find("{");
		size_t close_pos = block.rfind("}");
		std::string event_block = block.substr(open_pos + 1, close_pos - open_pos - 1);
		parseSimpleDirective(this->event_directive, event_block);
	} else if (block_name == "http") {
		// http_directive.parsing(block);
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
		blocks = ft_split(config_text, "\n");
	} else {
		parseSimpleDirective(main_directive, config_text.substr(0, pos));
		blocks = ft_split(config_text.substr(pos + strlen(MAIN_SEPARATOR)), "\n");
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		identifyBlock(blocks[i]);
	}
	return SUCCESS;
}

void	Config::printConfig()
{
	std::cout << this->config_text << std::endl;
}
