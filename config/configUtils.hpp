#ifndef CONFIGUTILS_HPP
#define CONFIGUTILS_HPP

#include "GlobalConfig.hpp"
#include "exceptionType.hpp"
#include "webserv.hpp"

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

struct CommonDirective
{
	std::string _root;
	bool _autoindex;
	std::vector<std::string> _index;
	std::map<std::string, std::string> _error_page;
	std::vector<std::string> _limit_except;
	int _client_limit_body_size;
	int _request_limit_header_size;
	std::map<std::string, std::string> _cgi_path;

	CommonDirective() : _autoindex(false), _client_limit_body_size(1000000),
						_request_limit_header_size(4000)
	{
		_limit_except.push_back("GET");
		_limit_except.push_back("POST");
		_limit_except.push_back("DELETE");
	}

	CommonDirective(CommonDirective const &cd)
		: _root(cd._root), _autoindex(cd._autoindex), _index(cd._index), _error_page(cd._error_page),
		_limit_except(cd._limit_except), _client_limit_body_size(cd._client_limit_body_size), 
		_request_limit_header_size(cd._request_limit_header_size), _cgi_path(cd._cgi_path){}

	CommonDirective& operator=(CommonDirective const &cd) {
		if (this == &cd) {
			return *this;
		}
		_root = cd._root;
		_autoindex = cd._autoindex;
		_index = cd._index;
		_error_page = cd._error_page;
		_limit_except = cd._limit_except;
		_client_limit_body_size = cd._client_limit_body_size;
		_cgi_path = cd._cgi_path;
		return *this;
	}
};

bool isCommonDirective(std::string key);
void getCommonRoot(CommonDirective &directive, std::vector<std::string> const &line);
void getCommonAutoIndex(CommonDirective &directive, std::vector<std::string> const &line);
void getCommonIndex(CommonDirective &directive, std::vector<std::string> const &line);
void getCommonErrorPage(CommonDirective &directive, std::vector<std::string> const &line);
void getCommonCgiPath(CommonDirective &directive, std::vector<std::string> const &line);
void getClientBodySize(CommonDirective &directive, std::vector<std::string> const &line);
void getClientHeaderSize(CommonDirective &directive, std::vector<std::string> const &line);
void getAllowedMethod(CommonDirective &directive, std::vector<std::string> const &line);
void parseCommonDirective(CommonDirective &directive, std::vector<std::string> const &line);
void parseSimpleDirective(std::map<std::string, std::string> &simple, CommonDirective &common, std::string const &buffer);
std::string sperateBrace(std::string const &buffer);
std::string getBlockName(std::string const &block);
std::string getBlockContent(std::string const &block);

#endif
