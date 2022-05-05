#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int fd, ConfigServer server_info) : Socket(CLIENT_SOCKET, fd), server_info(server_info)
{

};

ClientSocket::~ClientSocket()
{

};

ConfigServer ClientSocket::getConnectServerInfo() { return server_info; }