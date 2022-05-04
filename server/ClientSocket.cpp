#include "ClientSocket.hpp"

ClientSocket::ClientSocket() : Socket(CLIENT_SOCKET)
{
	
};
ClientSocket::ClientSocket(int fd) : Socket(CLIENT_SOCKET, fd)
{

};

ClientSocket::~ClientSocket()
{

};
