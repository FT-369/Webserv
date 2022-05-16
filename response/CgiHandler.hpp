#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include "ConfigServer.hpp"
#include "Request.hpp"

class CgiHandler
{
private:
	std::map<std::string, std::string> _cgi_env;
	ConfigLocation *_location_info;
	Request *_request;

public:
	CgiHandler(ConfigLocation *location_info, Request *req);
	~CgiHandler();
	void cgiInitEnv();
	char **convertEnv();
	int executeCgi();

private:
	CgiHandler();

};


#endif