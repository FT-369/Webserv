#include "Server.hpp"
#include <iosfwd>

#define BUFFERSIZE 1024

int num = 0;
Server::Server() {}

Server::Server(Config &con) : _config(con) {}

Server::~Server() {}

KqueueHandler &Server::getKqHandler()
{
	return _kq;
}
void Server::setConfig(Config config)
{
	_config = config;
}

void Server::serverConnect()
{
	std::vector<ConfigServer> servers = (_config.getHttpDirective()).getServers();
	int server_size = servers.size();
	for (int i = 0; i < server_size; i++)
	{
		ServerSocket *new_socket;
		try
		{
			new_socket = new ServerSocket(servers[i]);
			new_socket->binding();
		}
		catch (const std::exception &e)
		{
			delete new_socket;
			new_socket = NULL;
			std::cerr << e.what() << '\n';
			continue;
		}
		_socket[new_socket->getSocketFd()] = new_socket;
		_kq.addEvent(EVFILT_READ, new_socket->getSocketFd(), NULL);
	}
}

void Server::acceptGetClientFd(ServerSocket *server_socket)
{
	int connect_fd;

	server_socket->clientAccept(connect_fd);
	fcntl(connect_fd, F_SETFL, O_NONBLOCK);
	ClientSocket *new_socket = new ClientSocket(connect_fd, server_socket->getServerInfo());
	_socket[new_socket->getSocketFd()] = new_socket;
	_kq.addEvent(EVFILT_READ, connect_fd, NULL);
	_kq.addEvent(EVFILT_WRITE, connect_fd, NULL);
}

void Server::keventProcess()
{
	serverConnect(); // config server의 서버별 kqueue등록
	if ((_kq._kq_fd = kqueue()) == -1)
	{
		kqueue_error();
	}
	while (1)
	{
		int event_num = _kq.initKevent();
		if (event_num == -1)
		{
			continue;
		}
		for (int i = 0; i < event_num; i++)
		{
			std::map<uintptr_t, Socket *>::iterator it = _socket.find(_kq._event_list[i].ident);
			if (it == _socket.end()) // Server의 socket 리스트에 저장되지 않은 fd면 넘기기
				continue;
			if (_kq._event_list[i].flags & EV_ERROR)
			{
				ClientSocket *client_socket = dynamic_cast<ClientSocket *>(_socket[_kq._event_list[i].ident]);
				if (client_socket != 0 && static_cast<uintptr_t>(client_socket->getSocketFd()) == _kq._event_list[i].ident)
				{
					if (client_socket->getResource()->getPid() == -1)
					{
						if (client_socket->getRequest())
							fclose(client_socket->getRequest()->getSocketReadFP());
						if (client_socket->getResponse())
							fclose(client_socket->getResponse()->getSocketWriteFD());
					}
				}
				close(_kq._event_list[i].ident);
				continue;
			}
			switch (_kq._event_list[i].filter)
			{
			case EVFILT_READ:
			{
				switch (_socket[_kq._event_list[i].ident]->getSocketType())
				{
				case SERVER_SOCKET:
				{
					ServerSocket *server_socket = dynamic_cast<ServerSocket *>(_socket[_kq._event_list[i].ident]);
					try
					{
						acceptGetClientFd(server_socket);
					}
					catch (const std::exception &e)
					{
						std::cerr << e.what() << '\n';
					}
					continue;
				}
				case CLIENT_SOCKET:
				{
					ClientSocket *client_socket = dynamic_cast<ClientSocket *>(_socket[_kq._event_list[i].ident]);

					if (client_socket == 0)
					{
						continue;
					}
					if (static_cast<uintptr_t>(client_socket->getSocketFd()) == _kq._event_list[i].ident && client_socket->getStage() == GET_REQUEST)
					{
						if (client_socket->getRequest() == 0)
						{
							client_socket->setErrorResource("500");
							continue;
						}
						while (client_socket->getStage() != END_OF_REQUEST)
						{
							try
							{
								client_socket->readRequest();
							}
							catch (const std::exception &e)
							{
								client_socket->setRequestParseError(true);
								std::cerr << e.what() << '\n';
								break;
							}
						}
						client_socket->setStage(SET_RESOURCE);
						client_socket->setResourceFd();
						int read_fd = client_socket->getResource()->getReadFd();
						int write_fd = client_socket->getResource()->getWriteFd();
						if (client_socket->getStage() == SET_RESOURCE || client_socket->getStage() == CGI_WRITE)
						{
							if (read_fd != -1)
							{
								fcntl(read_fd, F_SETFL, O_NONBLOCK);
								_kq.addEvent(EVFILT_READ, read_fd, NULL);
								_socket[read_fd] = client_socket;
							}
							if (write_fd != -1)
							{
								fcntl(write_fd, F_SETFL, O_NONBLOCK);
								_kq.addEvent(EVFILT_WRITE, write_fd, NULL);
								_socket[write_fd] = client_socket;
							}
						}
					}
					else if (client_socket->getResource() != 0 && static_cast<uintptr_t>(client_socket->getResource()->getReadFd()) == _kq._event_list[i].ident && client_socket->getStage() == CGI_READ)
					{
						int stat, ret;
						char buffer[BUFFERSIZE];
						memset(buffer, 0, BUFFERSIZE);
						if (client_socket->getResource()->getPid() > 0)
						{ // cgi
							ret = waitpid(client_socket->getResource()->getPid(), &stat, WNOHANG);
							if (ret == 0)
							{
								continue;
								// process not finished yet
							}
							if (WIFSIGNALED(stat) == true)
							{
								// cgi error
								_socket.erase(_kq._event_list[i].ident);
								close(_kq._event_list[i].ident);
								continue;
							}
							else if (WIFEXITED(stat) == true)
							{
								int n = read(_kq._event_list[i].ident, buffer, BUFFERSIZE - 1);
								if (n < 0)
								{
									client_socket->setErrorResource("500");
									continue;
									// read error
								}
								buffer[n] = 0;
								client_socket->getResource()->getResourceContent().append(buffer, n);
								if (n < BUFFERSIZE - 1)
								{
									client_socket->parsingCGIResponse();
									close(client_socket->getResource()->getReadFd());
									close(client_socket->getResource()->getWriteFd());
									client_socket->setStage(SEND_RESPONSE);
								}
							}
						}
					}
					else if (client_socket->getResource() != 0 && static_cast<uintptr_t>(client_socket->getResource()->getReadFd()) == _kq._event_list[i].ident && client_socket->getStage() == SET_RESOURCE)
					{ // file 읽기
						FILE *resource_ptr = fdopen(_kq._event_list[i].ident, "r");
						fseek(resource_ptr, 0, SEEK_END);
						long resource_size = ftell(resource_ptr);
						rewind(resource_ptr);
						long total_size = 0;
						long fread_size = 0;
						while (total_size < resource_size)
						{
							char buf2[BUFFERSIZE];
							fread_size = fread(buf2, sizeof(char), BUFFERSIZE - 1, resource_ptr);
							if (fread_size < 0)
							{
								break;
							}
							total_size += fread_size;
							client_socket->getResource()->getResourceContent().append(buf2, fread_size);
						}
						fclose(resource_ptr);
						close(client_socket->getResource()->getReadFd());
						if (fread_size < 0)
						{
							client_socket->setErrorResource("500");
							continue;
						}
						else if (total_size != resource_size)
						{
							client_socket->setErrorResource("500");
							continue;
						}
						client_socket->getResource()->setResourceLength(resource_size);
						client_socket->setStage(SEND_RESPONSE);
					}
					break;
				}
				}
				break;
			}
			case EVFILT_WRITE:
			{
				switch (_socket[_kq._event_list[i].ident]->getSocketType())
				{
				case CLIENT_SOCKET:
				{
					ClientSocket *client_socket = dynamic_cast<ClientSocket *>(_socket[_kq._event_list[i].ident]);
					// response 보내주는거
					if (client_socket != 0 && static_cast<uintptr_t>(client_socket->getSocketFd()) == _kq._event_list[i].ident && client_socket->getRequest() != 0 && client_socket->getStage() == SEND_RESPONSE) // + 리소스도 다 읽었으면
					{
						client_socket->makeResponse();
						client_socket->sendResponse();
						_socket[_kq._event_list[i].ident] = NULL;
						delete client_socket;
						client_socket = NULL;
						_socket.erase(_kq._event_list[i].ident);
						close(_kq._event_list[i].ident);
					}
					else if (client_socket != 0 && client_socket->getStage() == CGI_WRITE && static_cast<uintptr_t>(client_socket->getResource()->getWriteFd()) == _kq._event_list[i].ident)
					{
						FILE *resource_ptr = fdopen(client_socket->getResource()->getWriteFd(), "wb");
						fwrite(client_socket->getRequest()->getRequestBody().c_str(), 1, client_socket->getRequest()->getRequestBody().size(), resource_ptr);
						fclose(resource_ptr);
						_socket.erase(client_socket->getResource()->getWriteFd());
						client_socket->setStage(CGI_READ);
					}
					else if (client_socket != 0 && client_socket->getResource() != 0 && static_cast<uintptr_t>(client_socket->getResource()->getWriteFd()) == _kq._event_list[i].ident && client_socket->getStage() == SET_RESOURCE)
					{
						// POST
						FILE *resource_ptr = fdopen(client_socket->getResource()->getWriteFd(), "wb");
						fwrite(client_socket->getRequest()->getRequestBody().c_str(), 1, client_socket->getRequest()->getRequestBody().size(), resource_ptr);
						fclose(resource_ptr);
						close(client_socket->getResource()->getWriteFd());
						client_socket->setStage(SEND_RESPONSE);
					}
				}
				}
				break;
			}
			}
		}
	}
}
