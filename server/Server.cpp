#include "Server.hpp"
#include "Request.hpp"
#include <stdio.h>
#include <sys/event.h>

Server::Server() {}

Server::Server(Config &con) : _config(con) {}

Server::~Server() {}

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
    // kq.saved_fd[connect_fd] = "";
}

ServerSocket *Server::isServerFd(uintptr_t fd)
{
    if (_socket[fd]->getSocketType() == SERVER_SOCKET)
    {
        return dynamic_cast<ServerSocket *>(_socket[fd]);
    }
    return NULL;
}

void Server::keventProcess()
{
    // std::map<int, Request *> request_map;
    serverConnect();
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
                std::map<uintptr_t, Socket *>::iterator it = _socket.find(_kq._event_list[i].ident);
                if (it == _socket.end()) // Server의 socket 리스트에 저장되지 않은 fd면 넘기기
                    continue;
                ServerSocket *server_socket = isServerFd(_kq._event_list[i].ident);
                std::cout << server_socket << std::endl;
                if (server_socket) // fd가 서버 소켓이면 클라이언트 accept
                {
                    acceptGetClientFd(server_socket);
                }
                else
                {
                    std::cout << "EVFILT_READ" << std::endl;
                    ClientSocket *cs = dynamic_cast<ClientSocket *>(_socket[_kq._event_list[i].ident]);

                    if (cs->getRequest() == 0)
                    {
                        std::cerr << "Request() error" << std::endl;
                        continue;
                    }
                    while (cs->getRequestStatus() != READ_END_OF_REQUEST)
                    {
                        if (cs->recieveRequest() == ERROR)
                        {
                            std::cerr << "parseRequest() error" << std::endl;
                            continue;
                        }
                    }
                    // request_map[kq.event_list[i].ident] = cs->getRequest();
                    // if (MakeResponseMsg() == ERROR); // 만들어진 response를 어떤식으로 해당 fd에 저장할지 고민이 필요
                    //     throw error;
                }
            }
            case EVFILT_WRITE:
            {
                std::map<uintptr_t, Socket *>::iterator it = _socket.find(_kq._event_list[i].ident);
                if (it == _socket.end()) // Server의 socket 리스트에 저장되지 않은 fd면 넘기기
                    continue;
                ClientSocket *cs = dynamic_cast<ClientSocket *>(_socket[_kq._event_list[i].ident]);
                if (cs != 0 && cs->getRequest() != 0 && cs->getRequestStatus() == READ_END_OF_REQUEST)
                {
                    std::cout << "EVFILT_WRITE - fd[" << _kq._event_list[i].ident << "]: " << cs << std::endl;
                    cs->sendResponse();
                    _socket.erase(_kq._event_list[i].ident);
                    close(_kq._event_list[i].ident);
                }
            }
            }
        }
    }
}
