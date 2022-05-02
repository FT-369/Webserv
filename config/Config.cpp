#include "Config.hpp"

Config::Config() : filename(DEFALUT_CONF), config_text(""), default_type("text/plain")
{
	parsingConfig();
	parsingMimeTypes(MIME_TYPES);
}

Config::Config(std::string const &filename) : filename(filename), config_text(""), default_type("text/plain")
{
	parsingConfig();
	parsingMimeTypes(MIME_TYPES);
}

Config::~Config() { }

std::string Config::getFileName() { return filename; }
std::string Config::getConfigText() { return config_text; }
std::map<std::string, std::string> Config::getMimeTypes() { return mime_types; }
std::map<std::string, std::string> Config::getGeneralDirective() { return general_directive; }
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

int Config::identifyHttpBlock(std::string const &block)
{
	std::string block_name = getBlockName(block);
	std::string block_content = getBlockContent(block);

	if (block_name == "http") {
		http_directive.parsingHttp(block_content);
	} else if (block_name != "") {
		// return ERROR;
	}
	return SUCCESS;
}

int Config::parseGeneralDirective(std::map<std::string, std::string> &directive, std::string const &buffer) {
	std::vector<std::string> main_line;
	std::vector<std::string> split_line;
	std::string	value;

	main_line = ft_split(buffer, ";");
	for (int i = 0; i < main_line.size(); i++)
	{
		split_line = ft_split_space(main_line[i]);
		if (split_line.size() < 2)
			return ERROR;
		value = split_line[1];
		for (size_t i = 2; i < split_line.size(); i++)
			value += " " + split_line[i];
		directive[split_line[0]] = value;
	}
	return SUCCESS;
}

int Config::parsingConfig()
{
	size_t pos;
	std::vector<std::string> blocks;

	if (readConfigFile() == ERROR)
		return ERROR;
	
	pos = config_text.find(MAIN_SEPARATOR);
	if (pos == std::string::npos) {
		blocks = ft_split(config_text, BLOCK_SEPARATOR);
	} else {
		parseGeneralDirective(general_directive, config_text.substr(0, pos));
		blocks = ft_split(config_text.substr(pos + strlen(MAIN_SEPARATOR)), BLOCK_SEPARATOR);
	}

	for (size_t i = 0; i < blocks.size(); i++) {
		identifyHttpBlock(blocks[i]);
	}
	return SUCCESS;
}

int Config::parsingMimeTypes(std::string const &filename) {
	std::ifstream is(filename);
	std::string buffer;
	std::string line = "";
	std::vector<std::string> split_line;

	if (is.fail()) {
		std::cerr << "Unable to find the file: " << filename << std::endl;
		return ERROR;
	}
	while (std::getline(is, buffer)) {
		line += buffer;
	}
	split_line = ft_split(line, ";");

	for (size_t i = 0; i < split_line.size(); i++) {
		std::vector<std::string> key_value = ft_split_space(split_line[i]);
		if (key_value.size() < 2) {
			return ERROR;
		}
		for (size_t i = 1; i < key_value.size(); i++) {
			mime_types[key_value[i]] = key_value[0];
		}
	}
	return SUCCESS;
}

int Config::parsingMimeTypes(std::string const &filename) {
	std::ifstream is(filename);
	std::string buffer;
	std::string line = "";
	std::vector<std::string> split_line;

	if (is.fail()) {
		std::cerr << "Unable to find the file: " << filename << std::endl;
		return ERROR;
	}
	while (std::getline(is, buffer)) {
		line += buffer;
	}
	split_line = ft_split(line, ";");

	for (size_t i = 0; i < split_line.size(); i++) {
		std::vector<std::string> key_value = ft_split_space(split_line[i]);
		if (key_value.size() < 2) {
			return ERROR;
		}
		for (size_t i = 1; i < key_value.size(); i++) {
			mime_types[key_value[i]] = key_value[0];
		}
	}
	return SUCCESS;
}

int	Config::setStatusCode()
{
	status_code[100] = "Continue";
	status_code[101] = "Switching Protocols";
	status_code[200] = "OK";
	status_code[201] = "Created";
	status_code[202] = "Accepted";
	status_code[203] = "Non-Authoritative";
	status_code[204] = "No Content";
	status_code[205] = "Reset Content";
	status_code[206] = "Partial Content";
	status_code[300] = "Multiple Choices";
	status_code[301] = "Moved Permanently";
	status_code[302] = "Found";
	status_code[303] = "See Other";
	status_code[304] = "Not Modified";
	status_code[305] = "Use Proxy";
	status_code[307] = "Temporary Redirect";
	status_code[400] = "Bad Request";
	status_code[401] = "Unauthorized";
	status_code[403] = "Forbidden";
	status_code[404] = "Not Found";
	status_code[405] = "Method Not Allowed";
	status_code[406] = "Not Acceptable";
	status_code[407] = "Proxy Authentication";
	status_code[408] = "Request Timeout";
	status_code[409] = "Conflict";
	status_code[410] = "Gone";
	status_code[411] = "Length Required";
	status_code[412] = "Precondition Failed";
	status_code[413] = "Request Entity Too Large";
	status_code[414] = "Request URI Too Long";
	status_code[415] = "Unsupported Media Type";
	status_code[416] = "Requested Range Not Satisfiable";
	status_code[500] = "Expectation Failed";
	status_code[501] = "Internal Server Error";
	status_code[502] = "Not Implemented";
	status_code[503] = "Bad Gateway";
	status_code[504] = "Gateway Timeout";
	status_code[505] = "HTTP Version Not Supported";
}
