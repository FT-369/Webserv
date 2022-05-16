#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int fd, ConfigServer server_info)
	: Socket(CLIENT_SOCKET, fd), _server_info(server_info), _request(new Request(fd)), _response(NULL)
{
	_response = new Response(_request);
};

ClientSocket::~ClientSocket(){

};

ConfigServer ClientSocket::getConnectServerInfo() { return _server_info; }

Request *ClientSocket::getRequest() const { return _request; }

Response *ClientSocket::getResponse() const { return _response; }

int ClientSocket::recieveRequest()
{
	return _request->parseRequest();
}

Status ClientSocket::getRequestStatus()
{
	return _request->getStatus();
}

void ClientSocket::sendResponse()
{
	_response->mappingPath(_server_info.getLocations());
	return _response->combineResponse();
}
