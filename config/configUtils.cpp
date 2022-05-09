#include "configUtils.hpp"

bool isCommonDirective(std::string key) {
	return (key == ROOT || key == AUTOINDEX || key == INDEX || key == ERROR_PAGE
		|| key == CLIENT_BODY_SIZE || key == CLIENT_HEADER_SIZE || key == CGI_PATH
		|| key == ALLOWED_METHOD);
}

int getCommonRoot(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 2)
		return ERROR;	// 지시어 형식이 맞지 않음
	directive.root = line[1];
	return SUCCESS;
}

int getCommonAutoIndex(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 2)
		return ERROR;	// 지시어 형식이 맞지 않음
	if (line[1] == "on") {
		directive.autoindex = true;
	} else if (line[1] == "off") {
		directive.autoindex = false;
	} else {
		return ERROR;	// !!! 올바른 value가 아님
	}
	return SUCCESS;
}

int getCommonIndex(CommonDirective &directive, std::vector<std::string> const &line) {
	directive.index = line;
	if (directive.index.empty())
		return ERROR;	// 지시어 형식이 맞지 않음
	directive.index.erase(directive.index.begin());
	return SUCCESS;
}

int getCommonErrorPage(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() < 3)
		return ERROR;	// 지시어 형식이 맞지 않음

	std::string error_page = line[line.size() - 1];
	for (int i = 1; i < line.size() - 1; i++) {
		char *endptr = 0;
		double d = std::strtod(line[i].c_str(), &endptr);
		int n = static_cast<int>(d);

		if (strlen(endptr) != 0 || d != n || d < 0.0)
			return ERROR;	// !!! 올바른 value가 아님
		directive.error_page[n] = error_page;
	}
	return SUCCESS;
}

int getCommonCgiPath(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 3)
		return ERROR;	// 지시어 형식이 맞지 않음
	directive.cgi_path[line[1]] = line[2];
	return SUCCESS;
}

int getClientBodySize(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 2)
		return ERROR;	// 지시어 형식이 맞지 않음

	char *endptr = 0;
	double d_body_size = std::strtod(line[1].c_str(), &endptr);
	int body_size = static_cast<int>(d_body_size);

	if (strlen(endptr) != 0  || d_body_size != body_size || d_body_size < 0.0)
		return ERROR;	// !!! 올바른 value가 아님
	directive.client_limit_body_size = body_size;
	return SUCCESS;
}

int getClientHeaderSize(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() != 2)
		return ERROR;	// 지시어 형식이 맞지 않음
	
	char *endptr = 0;
	double d_header_size = std::strtod(line[1].c_str(), &endptr);
	int header_size = static_cast<int>(d_header_size);

	if (strlen(endptr) != 0 || d_header_size != header_size || d_header_size < 0.0)	// !!! 올바른 value가 들어오는지 체크
		return ERROR;	// !!! 올바른 value가 아님
	directive.request_limit_header_size = header_size;
	return SUCCESS;
}

int getAllowedMethod(CommonDirective &directive, std::vector<std::string> const &line) {
	if (line.size() < 2)
		return ERROR;
	std::vector<std::string> new_directive;
	for (int i = 1; i < line.size(); i++)
	{
		if (line[i] != "GET" && line[i] != "POST" && line[i] != "DELETE")
			return ERROR;	// 지시어 형식이 맞지 않음
		new_directive.push_back(line[i]);
	}
	directive.limit_except = new_directive;
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
	} else if (key == ALLOWED_METHOD) {
		ret = getAllowedMethod(directive, line);
	} else {
		return ERROR;	// common directive에 해당하는 key값이 아님
	}
	return SUCCESS;
}

int parseSimpleDirective(std::map<std::string, std::string> &simple, CommonDirective &common, std::string const &buffer) {
	std::vector<std::string> main_line;
	std::vector<std::string> split_line;
	std::string	value;

	main_line = ft_split(buffer, ";");
	if (ft_trim(main_line[main_line.size() - 1]) != "") {
		return ERROR;	// buffer가 ;으로 끝나지 않음
	} else {
		main_line.pop_back();
	}

	for (int i = 0; i < main_line.size(); i++)
	{
		split_line = ft_split_space(main_line[i]);
		if (split_line.size() < 2)
			return ERROR;	// 지시어 형식이 맞지 않음
		
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
	std::string block_name;

	if (pos == std::string::npos)
		return "";	// block이 없어 block_name을 찾지 못함
	return ft_trim(block.substr(0, pos));
}

std::string getBlockContent(std::string const &block)
{
	size_t open_pos = block.find("{");
	size_t close_pos = block.rfind("}");

	if (open_pos == std::string::npos || close_pos == std::string::npos)
		return "";	// block이 없어 block_content를 찾지 못함
	return block.substr(open_pos + 1, close_pos - open_pos - 1);
}

