#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

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
	void makePostHeader();
	void mappingPath();
	void makeEntity(std::string file);
	void makeResponse();
	void combineResponse();
	int isDirectory(const std::string &path);
	int isFile(const std::string &path);

	void makeGetResponse();
	void makePostResponse();
	void makeDeleteResponse();
	void makeErrorResponse(std::string error_num);
	void makeAutoIndex(std::string directory, DIR* dir);
	void settingRoute();
	std::string getContentType(std::string file);
	void mappingPath(std::vector<ConfigLocation> loacations);
};

#endif
