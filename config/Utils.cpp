#include "Utils.hpp"

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
