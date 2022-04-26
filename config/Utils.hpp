#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <cctype>

#define ERROR 1
#define SUCCESS 0

std::vector<std::string> ft_split(std::string const &line, std::string const &delimiter);
std::vector<std::string> ft_split_space(std::string const &line);
int parseSimpleDirective(std::map<std::string, std::string> &directive, const std::string &buffer);

#endif
