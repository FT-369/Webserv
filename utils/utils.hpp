#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <iostream>
#include <string>
#include <cctype>

#define ERROR 1
#define SUCCESS 0

int	error_handling(std::string error, int ret);
std::string ft_ltrim(std::string const &str);
std::string ft_rtrim(std::string const &str);
std::string ft_trim(std::string const &str);
std::vector<std::string> ft_split(std::string const &line, std::string const &delimiter);
std::vector<std::string> ft_split_space(std::string const &line);

#endif
