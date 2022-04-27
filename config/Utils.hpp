#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <cctype>
#include <stack>

#define ERROR 1
#define SUCCESS 0
#define MAIN_SEPARATOR "\n\n"
#define BLOCK_SEPARATOR "\n"


std::string ft_ltrim(std::string const &str);
std::string ft_rtrim(std::string const &str);
std::string ft_trim(std::string const &str);
std::vector<std::string> ft_split(std::string const &line, std::string const &delimiter);
std::vector<std::string> ft_split_space(std::string const &line);
int parseSimpleDirective(std::map<std::string, std::string> &directive, const std::string &buffer);
std::string sperateBrace(std::string const &buffer);
std::string getBlockName(std::string const &block);
std::string getBlockContent(std::string const &block);

#endif
