#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
class Response
{
private:
	std::string start_line;
	std::map<std::string, std::string> header;
	std::string entity;
	std::string status;
	std::map<std::string, std::string> status_code;
	Request *request;
	Config config;

public:
	Response(Config const &config);
	Response(Config const &config, Request *request);
	~Response();

	void	setStatusCode();
	void	makeStartLine();
	void	makeHeader();
	void	makeEntity();
	std::string	makeResponse();
	std::string	getContentType(std::string file);
};

#endif
