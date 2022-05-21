#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "CgiHandler.hpp"
#include "Resource.hpp"
#include "webserv.hpp"
class Response
{
private:
	FILE* _socket_write;
	std::string _start_line;
	std::map<std::string, std::string> _header;
	std::string _entity;
	std::string _status_code;
	Request *_request;
	Resource *_resource;

public:
	Response(Request *request);
	~Response();
	FILE* getSocketWriteFD() const;

	void makeStartLine();
	void setRedirect();
	void makeHeader();
	void makePostHeader();
	void mappingPath();
	void makeEntity(std::string file);
	void makeResponse();
	void combineResponse();
	void setStatusCode(std::string const &status_code);
	void makeGetResponse();
	void makePostResponse();
	void makeDeleteResponse();
	void makeErrorResponse(std::string error_num);
	void makeAutoIndex(std::string directory, DIR* dir);
	// void settingRoute();
	std::string getContentType(std::string file);
	// void mappingPath(std::vector<ConfigLocation> const &locations);
};

#endif
