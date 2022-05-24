#include "Server.hpp"

#define BUFFERSIZE 1024

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
	std::cout << "origin_ server_size = " << (_config.getHttpDirective()).getServers().size() << std::endl;
	std::cout << "server_size = " << server_size << std::endl;
	for (int i = 0; i < server_size; i++)
	{
		int error_flag;
		ServerSocket *new_socket = new ServerSocket(servers[i]);
		std::cout << "server host:" << new_socket->getSocketHost() << std::endl;
		std::cout << "server port:" << new_socket->getSocketPort() << std::endl;

		if (new_socket->binding() == ERROR)
			continue;
		_socket[new_socket->getSocketFd()] = new_socket;
		_kq.addEvent(EVFILT_READ, new_socket->getSocketFd(), NULL);
	}
	std::cout << "map_socket_size: " << _socket.size() << std::endl;
}

void Server::acceptGetClientFd(ServerSocket *server_socket)
{
	int connect_fd;

	server_socket->clientAccept(connect_fd);
	fcntl(connect_fd, F_SETFL, O_NONBLOCK);
	ClientSocket *new_socket = new ClientSocket(connect_fd, server_socket->getServerInfo());
	_socket[new_socket->getSocketFd()] = new_socket;
	std::cout << "new_socket = " << new_socket->getSocketFd() << std::endl;
	_kq.addEvent(EVFILT_READ, connect_fd, NULL);
	_kq.addEvent(EVFILT_WRITE, connect_fd, NULL);
}

void Server::keventProcess()
{
	serverConnect(); // config server의 서버별 kqueue등록
	if ((_kq._kq_fd = kqueue()) == -1)
		std::cout << " errorororororo" << std::endl;
	while (1)
	{
		int event_num = _kq.initKevent();
		if (event_num == -1)
		{
			std::cout << "errrororroroor" << std::endl;
			exit(1);
		}
		for (int i = 0; i < event_num; i++)
		{
			std::map<uintptr_t, Socket *>::iterator it = _socket.find(_kq._event_list[i].ident);
			if (it == _socket.end()) // Server의 socket 리스트에 저장되지 않은 fd면 넘기기
				continue;
			if (_kq._event_list[i].flags == EV_ERROR)
			{
				std::cout << "errorororor" << std::endl;
				_kq.disableEvent(_kq._event_list[i].filter, EVFILT_WRITE, NULL); //에러가 발생한 fd 삭제
				_socket.erase(_kq._event_list[i].ident);
				continue;
			}
			switch (_kq._event_list[i].filter)
			{
			case EVFILT_READ:
			{
				if (it == _socket.end()) // Server의 socket 리스트에 저장되지 않은 fd면 넘기기
					continue;
				switch (_socket[_kq._event_list[i].ident]->getSocketType())
				{
				case SERVER_SOCKET:
				{
					ServerSocket *server_socket = dynamic_cast<ServerSocket *>(_socket[_kq._event_list[i].ident]);
					std::cout << server_socket << std::endl;
					acceptGetClientFd(server_socket);
				}
				case CLIENT_SOCKET:
				{
					ClientSocket *client_socket = dynamic_cast<ClientSocket *>(_socket[_kq._event_list[i].ident]);
					std::cout << client_socket << std::endl;
					int error = 0;

					if (client_socket->getStage() == GET_REQUEST)
					{
						if (client_socket->getRequest() == 0)
						{
							std::cerr << "Request() error" << std::endl;
							continue;
						}
						while (client_socket->getStage() != END_OF_REQUEST)
						{
							if (client_socket->recieveRequest() == ERROR)
							{
								std::cerr << "parseRequest() error" << std::endl;
								error = 1;
								break;
							}
						}
						if (error)
						{
							std::cout << "request error" << std::endl;
							continue;
						}
						client_socket->setStage(SET_RESOURCE);
						client_socket->setResourceFd();
						int read_fd = client_socket->getResource()->getReadFd();
						int write_fd = client_socket->getResource()->getWriteFd();
						if (read_fd != -1)
						{
							_kq.addEvent(EVFILT_READ, read_fd, NULL);
							_socket[read_fd] = client_socket;
						}
						if (write_fd != -1)
						{
							_kq.addEvent(EVFILT_WRITE, write_fd, NULL);
							_socket[write_fd] = client_socket;
						}
					}
					else if (client_socket->getStage() == SET_RESOURCE)
					{
						int stat, ret, n;
						char buffer[BUFFERSIZE];
						memset(buffer, 0, BUFFERSIZE);
						if (client_socket->getResource()->getPid() > 0)
						{ // cgi
							ret = waitpid(client_socket->getResource()->getPid(), &stat, WNOHANG);
							if (ret < 0)
							{
								std::cout << "process error!!!" << std::endl;
								_kq.removeEvent(EV_DELETE, client_socket->getResource()->getReadFd(), NULL);
								// _socket.erase(client_socket->getResource()->getReadFd());
								close(client_socket->getResource()->getReadFd());
								continue;
								// process error
							}
							else if (ret == 0)
							{
								continue;
								// process not finished yet
							}
							if (WIFSIGNALED(stat) == true)
							{
								// cgi error
								std::cout << "cgi error!!!" << std::endl;
								_kq.removeEvent(EV_DELETE, client_socket->getResource()->getReadFd(), NULL);
								// _socket.erase(client_socket->getResource()->getReadFd());
								close(client_socket->getResource()->getReadFd());
								continue;
							}
							else if (WIFEXITED(stat) == true)
							{
								n = read(client_socket->getResource()->getReadFd(), buffer, BUFFERSIZE - 1);
								if (n < 0)
								{
									std::cout << "cgi read errorr !!!!!!!" << std::endl;
									continue;
									// read error
								}
								buffer[n] = 0;
								client_socket->getResource()->setContent(client_socket->getResource()->getContent() + std::string(buffer));
								if (n < BUFFERSIZE - 1)
								{
									client_socket->getResponse()->makeResponse();
									_kq.removeEvent(EV_DELETE, client_socket->getResource()->getReadFd(), NULL);
									// _socket.erase(client_socket->getResource()->getReadFd());
									close(client_socket->getResource()->getReadFd());
									client_socket->setStage(MAKE_RESPONSE);
								}
							}
						}
						else
						{ // file 읽기
							n = read(_kq._event_list[i].ident, buffer, BUFFERSIZE - 1);
							if (n < 0)
							{
								std::cout << "read errorr !!!!!!!" << std::endl;
								// read error;
								continue;
							}
							buffer[n] = 0;
							client_socket->getResource()->setContent(client_socket->getResource()->getContent() + std::string(buffer));
							if (n < BUFFERSIZE - 1)
							{
								client_socket->getResponse()->makeResponse();
								_kq.removeEvent(EV_DELETE, _kq._event_list[i].ident, NULL);
								_socket.erase(_kq._event_list[i].ident);
								close(_kq._event_list[i].ident);
								client_socket->setStage(MAKE_RESPONSE);
							}
						}
					}
				}
				}
			}
			case EVFILT_WRITE:
			{
				switch (_socket[_kq._event_list[i].ident]->getSocketType())
				{
				case CLIENT_SOCKET:
				{
					ClientSocket *client_socket = dynamic_cast<ClientSocket *>(_socket[_kq._event_list[i].ident]);
					// response 보내주는거
					if (client_socket != 0 && client_socket->getRequest() != 0 && client_socket->getStage() == MAKE_RESPONSE) // + 리소스도 다 읽었으면
					{
						std::cout << "EVFILT_WRITE - fd[" << _kq._event_list[i].ident << "]: " << client_socket << std::endl;
						client_socket->sendResponse();
						_socket.erase(_kq._event_list[i].ident);
						close(_kq._event_list[i].ident);
					}
					else if (client_socket->getStage() == SET_RESOURCE)
					{
						// cgi & POST
						// POST 요청 다 받아오면 파일(resource write_fd)에 request body를 한번에 쓰기
						int n = write(client_socket->getResource()->getWriteFd(), client_socket->getRequest()->getRequestBody().c_str(), client_socket->getRequest()->getRequestBody().size());
						if (n < 0)
						{
							// error
							std::cout << "write error" << std::endl;
							continue;
						}
						if (n < client_socket->getRequest()->getRequestBody().size())
						{
							client_socket->getRequest()->getRequestBody().erase(0, n);
						}
						else
						{
							client_socket->getResponse()->makeResponse();
							_kq.removeEvent(EV_DELETE, client_socket->getResource()->getWriteFd(), NULL);
							// _socket.erase(client_socket->getResource()->getWriteFd());
							close(client_socket->getResource()->getWriteFd());
							client_socket->setStage(MAKE_RESPONSE);
						}
					}
				}
				}
			}
			}
		}
	}
}
