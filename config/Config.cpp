#include "Config.hpp"

#define ERROR 1
#define SUCCESS 0
#define PATH "../setting/"

Config::Config() : filename("../setting/default.conf"), config_text("") {
	readConfigFile();
}

Config::Config(const std::string &_filename) : filename(PATH + _filename), config_text("") {
	readConfigFile();
}

void Config::cutComment(std::string &buffer) {
	size_t	pos = buffer.find("#");
	
	if (pos != std::string::npos) {
		buffer = buffer.substr(0, pos);
	}
}

int Config::parseMainDirective(const std::string &buffer) {
	// size_t rpos = buffer.rfind(';');
	// std::string temp = buffer.substr(0, rpos);
	std::vector<std::string> main_line;
	std::vector<std::string> temp_line;

	main_line = ft_split(buffer, ";");
	for (int i = 0; i < main_line.size(); i++)
	{
		temp_line = ft_split_space(main_line[i]);
		if (temp_line.size() != 2)
			return ERROR;
		main_directive[temp_line[0]] = temp_line[1];
	}
	return SUCCESS;
}

int Config::checkBrace(std::stack<bool> &check_brace, const std::string &buffer, bool &first_brace) {
	for (int i = 0; i < buffer.length(); i++)
	{
		if (buffer[i] == '{') {
			if (first_brace == false) {
				size_t rpos = config_text.rfind(';');
				parseMainDirective(config_text.substr(0, rpos));
				first_brace = true;
			}
			check_brace.push(1);
		}
		if (buffer[i] == '}') {
			if (check_brace.empty()) {
				config_text = "ERROR!";
				return ERROR;
			}
			check_brace.pop();
		}
	}
	return SUCCESS;
}

int Config::readConfigFile()
{
	std::ifstream is(filename);
	std::string buffer;
	std::stack<bool> check_brace;
	bool first_brace = false;

	if (is.fail())
	{
		std::cerr << "Unable to find the file: " << filename << std::endl;
		return ERROR;
	}
	while (std::getline(is, buffer))
	{
		cutComment(buffer);
		if (checkBrace(check_brace, buffer, first_brace)) {
			config_text = "ERROR!";
			return ERROR;
		}
		config_text += buffer + "\n";
	}
	if (!check_brace.empty()) {
		config_text = "ERROR!";
		return ERROR;
	}
	return SUCCESS;
}

int Config::findMain() {
	size_t pos = config_text.find("{");
	std::string temp = config_text.substr(0, pos);
	size_t rpos = temp.rfind(";", pos);

	temp = temp.substr(0, rpos);
}

int Config::identifyBlock()
{
	for (int i = 0; i < config_text.length(); i++){
		while (std::isspace(config_text[i]))
		{
			/* code */
		}
		
		config_text.substr(i, ' ');
		if (config_text[i] == '{'){
			
		}
	}
}

int Config::parsingConfig()
{
	return SUCCESS;
}

void	Config::printConfig()
{
	std::cout << this->config_text << std::endl;
}

int main(void)
{
	Config config;

	config.printConfig();
}
