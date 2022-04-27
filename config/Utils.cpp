#include "Utils.hpp"

std::string ft_ltrim(std::string const &str)
{
	size_t idx = 0;

	while (isspace(str[idx]))
		idx++;
	return str.substr(idx);
}

std::string ft_rtrim(std::string const &str)
{
	size_t idx = str.length() - 1;

	while (isspace(str[idx]))
		idx--;
	return str.substr(0, idx + 1);
}

std::string ft_trim(std::string const &str)
{
	std::string ltrim, rtrim;

	ltrim = ft_ltrim(str);
	rtrim = ft_rtrim(ltrim);
	return rtrim;
}

std::vector<std::string> ft_split(std::string const &line, std::string const &delimiter)
{
	std::vector<std::string> words;
	std::string new_line = line;
	size_t delimiter_len = delimiter.length();

	size_t pos;
	while ((pos = new_line.find(delimiter)) != std::string::npos) {
		words.push_back(new_line.substr(0, pos));
		new_line = new_line.substr(pos + delimiter_len);
	}
	words.push_back(new_line);

	return words;
}

std::vector<std::string> ft_split_space(std::string const &line)
{
	std::vector<std::string> words;
	std::string new_line = line;
	int i = 0;

	while (i < new_line.length()) {
		if (isspace(new_line[i])) {
			if (i > 0)
				words.push_back(new_line.substr(0, i));
			while (isspace(new_line[i]))
				i++;
			new_line = new_line.substr(i);
			i = 0;
		}
		i++;
	}
	if (new_line != "")
		words.push_back(new_line);
	return words;
}

int parseSimpleDirective(std::map<std::string, std::string> &directive, const std::string &buffer) {
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