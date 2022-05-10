#ifndef CONFIGUTILS_HPP
#define CONFIGUTILS_HPP

#include "utils.hpp"
#include <map>
#include <stack>
#include <cstdlib>
#include <vector>

#define MAIN_SEPARATOR "\n\n"
#define BLOCK_SEPARATOR "\n"
#define ROOT "root"
#define AUTOINDEX "autoindex"
#define INDEX "index"
#define ERROR_PAGE "error_page"
#define CLIENT_BODY_SIZE "client_limit_body_size"
#define CLIENT_HEADER_SIZE "request_limit_header_size"
#define CGI_PATH "cgi_path"
#define ALLOWED_METHOD "limit_except"


struct CommonDirective {
	std::string root;
	bool autoindex;
	std::vector<std::string> index;
	std::map<int, std::string> error_page;
	int client_limit_body_size;
	int request_limit_header_size;
	std::map<std::string, std::string> cgi_path;
	std::vector<std::string> limit_except;

	CommonDirective() : autoindex(false), client_limit_body_size(1000000), 
	request_limit_header_size(4000) {
		limit_except.push_back("GET");
		limit_except.push_back("POST");
		limit_except.push_back("DELETE");
	}
};

bool isCommonDirective(std::string key);
int getCommonRoot(CommonDirective &directive, std::vector<std::string> const &line);
int getCommonAutoIndex(CommonDirective &directive, std::vector<std::string> const &line);
int getCommonIndex(CommonDirective &directive, std::vector<std::string> const &line);
int getCommonErrorPage(CommonDirective &directive, std::vector<std::string> const &line);
int getCommonCgiPath(CommonDirective &directive, std::vector<std::string> const &line);
int getClientBodySize(CommonDirective &directive, std::vector<std::string> const &line);
int getClientHeaderSize(CommonDirective &directive, std::vector<std::string> const &line);
int getAllowedMethod(CommonDirective &directive, std::vector<std::string> const &line);
int parseCommonDirective(CommonDirective &directive, std::vector<std::string> const &line);
int parseSimpleDirective(std::map<std::string, std::string> &simple, CommonDirective &common, std::string const &buffer);
std::string sperateBrace(std::string const &buffer);
std::string getBlockName(std::string const &block);
std::string getBlockContent(std::string const &block);

#endif
