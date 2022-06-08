#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP
class CgiHandler;
#include "ConfigServer.hpp"
#include "Request.hpp"
#include "webserv.hpp"
#include "ClientSocket.hpp"

class CgiHandler
{
private:
	std::map<std::string, std::string> _cgi_env;
	ConfigLocation *_location_info;
	ClientSocket *_client_socket;

public:
	CgiHandler(ConfigLocation *location_info, ClientSocket *cs);
	~CgiHandler();
	void cgiInitEnv();
	char **convertEnv();
	int executeCgi();

private:
	CgiHandler();

};


#endif
