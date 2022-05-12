#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
#include <algorithm>
class Response
{
private:
	std::string _start_line;
	std::map<std::string, std::string> _header;
	std::string _entity;
	std::string _status;
	std::map<std::string, std::string> _status_code;
	Request *_request;
	std::map<std::string, std::string> _mime_types;
	std::vector<ConfigLocation> _locations;
	ConfigLocation *_route;
	std::string _file;

public:
	Response(std::map<std::string, std::string> const &mime_types, std::vector<ConfigLocation> routes);
	Response(std::map<std::string, std::string> const &mime_types, Request *request, std::vector<ConfigLocation> routes);
	~Response();

	void setStatusCode();
	void makeStartLine();
	void setRedirect();
	void makeHeader();
	void makeEntity(std::string file);
	std::string makeResponse();
	void makeGetResponse();
	void makeDeleteResponse();
	void makeErrorResponse(std::string error_num);
	std::string settingRoute();
	std::string getContentType(std::string file);
	std::map<std::string, std::string> getMimeType();
	void mappingPath();
};

#endif
