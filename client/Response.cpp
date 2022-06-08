#include "Response.hpp"

Response::Response(int socket_fd)
	: _socket_write(fdopen(dup(socket_fd), "w"))
{

}

Response::~Response()
{
	fclose(_socket_write);
}

FILE* Response::getSocketWriteFD() const { return _socket_write; }
std::string Response::getStatusCode() const { return _status_code; }
std::string &Response::getEntity() { return _entity; }
const std::string &Response::getEntity() const { return _entity; }

void Response::setStatusCode(std::string const &status_code) { _status_code = status_code; }
void Response::setEntity(std::string const &entity) { _entity = entity; }
unsigned long Response::getEntityLength() const { return _entity_length; }
void Response::setEntityLength(unsigned long length) { _entity_length = length; }

void Response::makeStartLine()
{
	_start_line = "HTTP/1.1 " + _status_code + " " + GlobalConfig::getStatusCode()[_status_code] + "\r\n";
}

void Response::makeHeader(Resource *resource)
{
	if (_header["Content-Type"] == "")
		_header["Content-Type"] = resource->getResourceType();
	if (_header["Content-Length"] == "")
		_header["Content-Length"] = std::to_string(_entity.length());
	_header["Server"] = "Mac Web Server";
}

void Response::makeRedirectHeader(ConfigLocation *route)
{
	if (route->getReturnCode() == "")
		return;
	_status_code = route->getReturnCode();
	_header["Location"] = route->getReturnData();
}


void Response::makeResponse(Resource *resource, ConfigLocation *route)
{
	// resource의 content 가져오기
	makeHeader(resource);
	makeRedirectHeader(route);
	makeStartLine();
}

std::string Response::combineResponse()
{
	std::string send_data;
	std::map<std::string, std::string>::iterator it;

	send_data += _start_line;
	std::cerr << send_data << std::endl;
	for (it = _header.begin(); it != _header.end(); it++)
	{
		send_data += it->first + ": " + it->second + "\r\n";
	}
	send_data += "\r\n";
	return send_data;
}

void Response::addHeader(std::string key, std::string value)
{
	_header[key] = value; 
}
