#include "configUtils.hpp"

bool isCommonDirective(std::string key) {
	return (key == ROOT || key == AUTOINDEX || key == INDEX || key == ERROR_PAGE
		|| key == CLIENT_BODY_SIZE || key == CLIENT_HEADER_SIZE || key == CGI_PATH);
}

int getCommonRoot(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 2)
		return ERROR;
	directive.root = line[1];
	return SUCCESS;
}

int getCommonAutoIndex(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 2)
		return ERROR;
	if (line[1] == "on") {
		directive.autoindex = true;
	} else {
		directive.autoindex = false;
	}
	return SUCCESS;
}

int getCommonIndex(CommonDirective &directive, std::vector<std::string> const &line) {
	directive.index = line;
	if (directive.index.empty())
		return ERROR;
	directive.index.erase(directive.index.begin());
	return SUCCESS;
}

int getCommonErrorPage(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() < 3)
			return ERROR;
	std::string error_page = line[line.size() - 1];
	for (int i = 1; i < line.size() - 1; i++) {
		char *endptr = 0;
		double n = std::strtod(line[i].c_str(), &endptr);
		if (std::string(endptr) != "" || n != static_cast<int>(n) || n < 0)
			return ERROR;
		directive.error_page[static_cast<int>(n)] = error_page;
	}
	return SUCCESS;
}

int getCommonCgiPath(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 3)
			return ERROR;
	directive.cgi_path[line[1]] = line[2];
	return SUCCESS;
}

int getClientBodySize(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 2)
			return ERROR;
	char *endptr = 0;
	double body_size = std::strtod(line[1].c_str(), &endptr);
	if (std::string(endptr) != "" || body_size != static_cast<int>(body_size) || body_size < 0)
			return ERROR;
	directive.client_limit_body_size = static_cast<int>(body_size);
	return SUCCESS;
}

int getClientHeaderSize(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 2)
			return ERROR;
	char *endptr = 0;
	double header_size = std::strtod(line[1].c_str(), &endptr);
	if (std::string(endptr) != "" || header_size != static_cast<int>(header_size) || header_size < 0)
			return ERROR;
	directive.request_limit_header_size = static_cast<int>(header_size);
	return SUCCESS;
}

int parseCommonDirective(CommonDirective &directive, std::vector<std::string> const &line) {
	int ret = SUCCESS;
	std::string key = line[0];

	if (key == ROOT) {
		ret = getCommonRoot(directive, line);
	} else if (key == AUTOINDEX) {
		ret = getCommonAutoIndex(directive, line);
	} else if (key == INDEX) {
		ret = getCommonIndex(directive, line);
	} else if (key == ERROR_PAGE) {
		ret = getCommonErrorPage(directive, line);
	} else if (key == CGI_PATH) {
		ret = getCommonCgiPath(directive, line);
	} else if (key == CLIENT_BODY_SIZE) {
		ret = getClientBodySize(directive, line);
	} else if (key == CLIENT_HEADER_SIZE) {
		ret = getClientHeaderSize(directive, line);
	}
	return SUCCESS;
}

int parseSimpleDirective(std::map<std::string, std::string> &simple, CommonDirective &common, std::string const &buffer) {
	std::vector<std::string> main_line;
	std::vector<std::string> split_line;
	std::string	value;

	main_line = ft_split(buffer, ";");
	for (int i = 0; i < main_line.size(); i++)
	{
		split_line = ft_split_space(main_line[i]);
		if (split_line.size() < 2)
			return ERROR;
		
		if (isCommonDirective(split_line[0])) {
			parseCommonDirective(common, split_line);
		} else {
			value = split_line[1];
			for (size_t i = 2; i < split_line.size(); i++)
				value += " " + split_line[i];
			simple[split_line[0]] = value;
		}
	}
	return SUCCESS;
}

std::string sperateBrace(std::string const &buffer) {
	std::stack<bool> check_brace;
	std::string modify_buffer;
	bool first_brace = false;
	int last_idx = -1;

	for (int i = 0; i < buffer.length(); i++)
	{
		modify_buffer += buffer[i];
		if (first_brace == false && buffer[i] == ';') {
			last_idx = i;
		}
		if (buffer[i] == '{') {
			first_brace = true;
			check_brace.push(1);
		} else if (buffer[i] == '}') {
			check_brace.pop();
			if (check_brace.empty()) {
				modify_buffer += BLOCK_SEPARATOR;
			}
		}
	}
	if (last_idx != -1)
		modify_buffer.insert(last_idx + 1, MAIN_SEPARATOR);
	return  modify_buffer;
}

std::string getBlockName(std::string const &block)
{
	size_t pos = block.find("{");

	return ft_trim(block.substr(0, pos));
}

std::string getBlockContent(std::string const &block)
{
	size_t open_pos = block.find("{");
	size_t close_pos = block.rfind("}");

	if (open_pos == std::string::npos || close_pos == std::string::npos)
		return "";
	return block.substr(open_pos + 1, close_pos - open_pos - 1);
}