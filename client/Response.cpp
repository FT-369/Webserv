#include "Response.hpp"

Response::Response(Request *request, Resource *resource)
	: _request(request), _resource(resource), _socket_write(NULL)
{
	_socket_write = fdopen(dup(_request->getSocketFD()), "w");
}

Response::~Response()
{
}

FILE* Response::getSocketWriteFD() const { return _socket_write; }
std::string Response::getStatusCode() const { return _status_code; }
std::string Response::getEntity() const { return _entity; }

void Response::setStatusCode(std::string const &status_code) { _status_code = status_code; }
void Response::setEntity(std::string const &entity) { _entity = entity; }

void Response::makeStartLine()
{
	_start_line = _request->_protocol + " " + _status_code + " " + GlobalConfig::getStatusCode()[_status_code] + "\r\n";
}

void Response::makePostHeader()
{
	_header["Server"] = "Mac Web Server";
}

void Response::makeGetHeader()
{
	if (_header["Content-Type"] == "")
		_header["Content-Type"] = _resource->getSrcContentType();
	if (_header["Content-Length"] == "")
		_header["Content-Length"] = std::to_string(_entity.length());
	_header["Server"] = "Mac Web Server";
}

void Response::makeRedirectHeader()
{
	if (_request->getRoute()->getReturnCode() == "")
		return;
	_status_code = _request->getRoute()->getReturnCode();
	_header["Location"] = _request->getRoute()->getReturnData();
}


void Response::makeResponse()
{
	// resource의 content 가져오기
	_entity = _resource->getContent();
	std::vector<std::string> temp;
	temp = _request->getRoute()->getCommonDirective()._limit_except;
	std::cout << "[Mapping Path] url: " << _request->getRoute()->getUrl() << ", file:" << _request->getFile() << std::endl;

	if (find(temp.begin(), temp.end(), _request->getMethod()) == temp.end())
	{
		std::cout << "ERROR Response " << std::endl;
		setStatusCode("405");
		makeGetHeader();
	}
	else if (_request->getMethod() == "GET")
	{
		std::cout << "getMethod GET" << std::endl;
		makeGetHeader();
	}
	else if (_request->getMethod() == "POST")
	{
		std::cout << "POST" << std::endl;
		makePostHeader();
	}
	else if (_request->getMethod() == "DELETE")
	{
		std::cout << "DELETE" << std::endl;
		makeGetHeader();
	}
	makeRedirectHeader();
	makeStartLine();
}

void Response::combineResponse()
{
	std::string send_data;
	std::map<std::string, std::string>::iterator it;

	send_data += _start_line;
	for (it = _header.begin(); it != _header.end(); it++)
	{
		send_data += it->first + ": " + it->second + "\r\n";
	}
	send_data += "\r\n" + _entity + "\r\n";

	fputs(send_data.c_str(), _socket_write);
	fflush(_socket_write);
	fclose(_socket_write);
}

void Response::addHeader(std::string key, std::string value)
{
	_header[key] = value; 
}
