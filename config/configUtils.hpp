#ifndef CONFIGUTILS_HPP
#define CONFIGUTILS_HPP

#include "utils.hpp"
#include <map>
#include <stack>

#define MAIN_SEPARATOR "\n\n"
#define BLOCK_SEPARATOR "\n"

int parseSimpleDirective(std::map<std::string, std::string> &directive, const std::string &buffer);
std::string sperateBrace(std::string const &buffer);
std::string getBlockName(std::string const &block);
std::string getBlockContent(std::string const &block);

#endif
