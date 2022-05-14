#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
#include <algorithm>

class Response
{
private:
	FILE* _socket_write;
	std::string _start_line;
	std::map<std::string, std::string> _header;
	std::string _entity;
	std::string _status;
	Request *_request;
	ConfigLocation *_route;
	std::string _file;

public:
	Response(Request *request);
	~Response();
	FILE* getSocketWriteFD();

	void makeStartLine();
	void setRedirect();
	void makeHeader();
	void makeEntity(std::string file);
	void makeResponse();
	void combineResponse();
	void makeGetResponse();
	void makeDeleteResponse();
	void makeErrorResponse(std::string error_num);
	std::string settingRoute();
	std::string getContentType(std::string file);
	void mappingPath(std::vector<ConfigLocation> loacations);
};

#endif
