#include "Utils.hpp"

std::vector<std::string> ft_split(std::string const line, std::string const delimiter)
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

std::vector<std::string> ft_split_space(std::string const line)
{
	std::vector<std::string> words;
	std::string new_line = line;
	int i = 0;

	while (i < new_line.length()) {
		if (isspace(new_line[i])) {
			words.push_back(new_line.substr(0, i));   
			while (isspace(new_line[i]))
				i++;
			new_line = new_line.substr(i);
			i = 0;
		}
		i++;
	}
	words.push_back(new_line);
	return words;
}
