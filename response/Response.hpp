#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "iostream"
class Response
{
private:
	std::string start_line;
	std::map<std::string, std::string> header;
	std::string entity;
	std::string status;
	std::map<std::string, std::string> status_code;
	Request *request;
	std::map<std::string, std::string> mime_types;
	std::vector<ConfigLocation> locations;
	ConfigLocation *route;
	std::string file;

public:
	Response(std::map<std::string, std::string> const &mime_types, std::vector<ConfigLocation> routes);
	Response(std::map<std::string, std::string> const &mime_types, Request *request, std::vector<ConfigLocation> routes);
	~Response();

	void	setStatusCode();
	void	makeStartLine();
	void	setRedirect();
	void	makeHeader();
	void	makeEntity();
	std::string makeResponse();
	void	makeDeleteResponse();
	void	makeGetResponse();
	void	makeErrorResponse(std::string error_num);
	std::string	getContentType(std::string file);
	std::map<std::string, std::string>	getMimeType();
	void mappingPath();
};

#endif
