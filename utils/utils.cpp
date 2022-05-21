#include "utils.hpp"

int	error_handling(std::string error, int ret)
{
	std::cerr << error << std::endl;
	return(ret);
}

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

int	isDirectory(const std::string &path)
{
	struct stat sb;

	if (stat(path.c_str(), &sb) != 0)
		return 0;
	if ((sb.st_mode & S_IFMT) & S_IFDIR)
		return 1;
	else 
		return 0;

}

int	isFile(const std::string &path)
{
	struct stat sb;
	
	if (stat(path.c_str(), &sb) != 0)
		return 0;
	if ((sb.st_mode & S_IFMT) & S_IFREG)
		return 1;
	else
		return 0;
}

int	getFileType(const std::string &path)
{
	struct stat sb;
	
	if (stat(path.c_str(), &sb) != 0)
		return 0;
	if ((sb.st_mode & S_IFMT) & S_IFDIR)
		return DIRECTORY;
	if ((sb.st_mode & S_IFMT) & S_IFREG)
		return FILE_TYPE;
	return 0;
}