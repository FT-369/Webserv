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
	void	makeHeader();
	void	makeEntity();
	std::string	makeGetResponse();
	std::string	getContentType(std::string file);
	std::map<std::string, std::string>	getMimeType();
	void mappingPath();
};

#endif
