#ifndef UTILS_HPP
#define UTILS_HPP

#include "webserv.hpp"
#include "GlobalConfig.hpp"
#define ERROR 1
#define SUCCESS 0
#define FILE_TYPE 1
#define DIRECTORY 2

std::string ft_ltrim(std::string const &str);
std::string ft_rtrim(std::string const &str);
std::string ft_trim(std::string const &str);
std::vector<std::string> ft_split(std::string const &line, std::string const &delimiter);
std::vector<std::string> ft_split_space(std::string const &line);
int	isDirectory(const std::string &path);
int	isFile(const std::string &path);
int	getFileType(const std::string &path);
std::string getExtension(std::string file);
std::string getContentType(std::string file);

#endif
