#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int fd, ConfigServer server_info)
	: Socket(CLIENT_SOCKET, fd), _server_info(server_info), _request(new Request(fd)), _response(NULL)
{
	_response = new Response(_request, server_info);
};

ClientSocket::~ClientSocket(){

};

ConfigServer ClientSocket::getConnectServerInfo() { return _server_info; }

Request *ClientSocket::getRequest() { return _request; }

Response *ClientSocket::getResponse() { return _response; }

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
	return _response->combineResponse();
}