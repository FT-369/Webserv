#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
#include <algorithm>
class Response
{
private:
	int _socket_fd;
	FILE* _socket_write;
	std::string _start_line;
	std::map<std::string, std::string> _header;
	std::string _entity;
	std::string _status;
	Request *_request;
	ConfigServer _server_info;
	ConfigLocation *_route;
	std::string _file;

public:
	Response(Request *request, ConfigServer server_info);
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
	void mappingPath();
};

#endif
