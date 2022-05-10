#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int fd, ConfigServer server_info)
	: Socket(CLIENT_SOCKET, fd), server_info(server_info), request(new Request(fd)), response(NULL)
{

};

ClientSocket::~ClientSocket()
{

};

ConfigServer ClientSocket::getConnectServerInfo() { return server_info; }

Request* ClientSocket::getRequest() { return request; }

Response* ClientSocket::getResponse() { return response; }
void ClientSocket::setResponse(Response* res) { response = res; }

int ClientSocket::recieveRequest()
{
	return request->parseRequest();
}

Status ClientSocket::getRequestStatus()
{
	return request->getStatus();
}