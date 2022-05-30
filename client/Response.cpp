#include "Response.hpp"

Response::Response(int socket_fd, Resource *resource)
	: _socket_write(fdopen(dup(socket_fd), "w"))
{

}

Response::~Response()
{
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

void Response::makePostHeader(Resource *resource)
{
	if (_header["Content-Type"] == "")
		_header["Content-Type"] = resource->getResourceType();
	if (_header["Content-Length"] == "")
		_header["Content-Length"] = std::to_string(_entity.length());
	_header["Server"] = "Mac Web Server";
}

void Response::makeGetHeader(Resource *resource)
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


void Response::makeResponse(Request *request, Resource *resource, ConfigLocation *route)
{
	// resource의 content 가져오기
	// _entity = _resource->getResourceContent();
	std::vector<std::string> temp;
	temp = route->getCommonDirective()._limit_except;

	if (find(temp.begin(), temp.end(), request->getMethod()) == temp.end())
	{
		std::cout << "ERROR Response " << std::endl;
		setStatusCode("405");
		makeGetHeader(resource);
	}
	else if (request->getMethod() == "GET")
	{
		std::cout << "getMethod GET" << std::endl;
		makeGetHeader(resource);
	}
	else if (request->getMethod() == "POST")
	{
		std::cout << "POST" << std::endl;
		makePostHeader(resource);
	}
	else if (request->getMethod() == "DELETE")
	{
		std::cout << "makeGetHeader" << std::endl;
		makeGetHeader(resource);
	}
	makeRedirectHeader(route);
	makeStartLine();
}

std::string Response::combineResponse()
{
	std::string send_data;
	std::map<std::string, std::string>::iterator it;

	send_data += _start_line;
	for (it = _header.begin(); it != _header.end(); it++)
	{
		send_data += it->first + ": " + it->second + "\r\n";
	}
	send_data += "\r\n";
	// send_data += "\r\n" + _entity + "\r\n";
	std::cerr << "send_data : " << send_data << std::endl;
	return send_data;
	// fputs(send_data.c_str(), _socket_write);
	// fflush(_socket_write);

	// fwrite(_resource->getResourceContent().c_str(), sizeof(char), (_resource->getResourceLength() > 0 ? _resource->getResourceLength() : _resource->getResourceContent().length()), _socket_write);
	// fclose(_socket_write);
}

void Response::addHeader(std::string key, std::string value)
{
	_header[key] = value; 
}
