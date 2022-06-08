#ifndef RESPONSE_HPP
#define RESPONSE_HPP
class Response;
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
	unsigned long _entity_length;
	std::string _status_code;

public:
	Response(int socket_fd);
	~Response();
	FILE* getSocketWriteFD() const;
	std::string getStatusCode() const;
	std::string &getEntity();
	const std::string &getEntity() const;
	unsigned long  getEntityLength() const;
	void setEntityLength(unsigned long length);

	void makeStartLine();
	void makeRedirectHeader(ConfigLocation *route);
	void makeHeader(Resource *resource);
	void setEntity(std::string const &entity);
	void makeResponse(Resource *resource, ConfigLocation *route);
	std::string combineResponse();
	void setStatusCode(std::string const &status_code);
	void addHeader(std::string key, std::string value);
};

#endif
