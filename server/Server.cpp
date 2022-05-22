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
	// std::map<int, Request *> request_map;
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

					if (client_socket->getRequest() == 0)
					{
						std::cerr << "Request() error" << std::endl;
						continue;
					}
					while (client_socket->getRequestStage() != READ_END_OF_REQUEST)
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
						continue;
					}
					else
					{
						client_socket->setResourceFd();
						int read_fd = client_socket->getResource()->getReadFd();
						int write_fd = client_socket->getResource()->getWriteFd();
						if (read_fd != -1)
						{
							_kq.addEvent(EVFILT_READ, read_fd, client_socket->getRequest());
							_socket[read_fd] = client_socket->getResource();
						}
						if (write_fd != -1)
						{
							_kq.addEvent(EVFILT_WRITE, write_fd, client_socket->getRequest());
							_socket[write_fd] = client_socket->getResource();
						}
					}
				}
				case RESOURCE_FD:
				{
					int stat, ret, n;
					char buffer[BUFFERSIZE];
					Resource *resource = dynamic_cast<Resource *>(_socket[_kq._event_list[i].ident]);
					if (resource->getPid() > 0)
					{ // cgi
						ret = waitpid(resource->getPid(), &stat, WNOHANG);
						if (ret < 0)
						{
							// process error
						}
						else if (ret == 0)
						{
							// process not finished yet
						}
						if (WIFSIGNALED(stat) == true)
						{
							// cgi error
						}
						else if (WIFEXITED(stat) == true)
						{
							n = read(resource->getReadFd(), buffer, BUFFERSIZE - 1);
							if (n < 0)
							{
								// read error
							}
						}
					}
					else
					{ // file 읽기
						n = read(_kq._event_list[i].ident, buffer, BUFFERSIZE - 1);
						if (n < BUFFERSIZE - 1)
						{
							buffer[n] = 0;
							resource->setContent(resource->getContent() + std::string(buffer));
						}
						else if (n < 0)
						{
							// read error
							continue;
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
					ClientSocket *cs = dynamic_cast<ClientSocket *>(_socket[_kq._event_list[i].ident]);
					if (cs != 0 && cs->getRequest() != 0 && cs->getRequestStage() == READ_END_OF_REQUEST) // + 리소스도 다 읽었으면
					{
						std::cout << "EVFILT_WRITE - fd[" << _kq._event_list[i].ident << "]: " << cs << std::endl;
						//  cs->_respose->sendResponse();
						_socket.erase(_kq._event_list[i].ident);
						close(_kq._event_list[i].ident);
					}
				}
				case RESOURCE_FD:
				{
					int ret;
					Resource *resource = dynamic_cast<Resource *>(_socket[_kq._event_list[i].ident]);
					// udata로 request 넣어주기
					if (resource->getPid() > 0) // cgi
					{
						// resource write_fd (cgi pipe) 에 쓰기 <- 정확히 뭘쓰는지??
					}
					else
					{
						// POST 요청 다 받아오면 파일(resource write_fd)에 request body를 한번에 쓰기
						// write(resource);
					}
				}
				}
			}
			}
		}
	}
}
