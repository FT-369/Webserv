#include "Server.hpp"
#include "Socket.hpp"
#include "ServerSocket.hpp"

Server::Server() {}

Server::Server(Config &config) : config(config) {}

Server::~Server() {}

void Server::serverConnect()
{
    std::vector<ConfigServer> servers = (config.getHttpDirective()).getServers();
    int server_size = servers.size();

    for (int i = 0; i < server_size; i++)
    {
        int error_flag;
        ServerSocket new_socket = ServerSocket(servers[i]);
        if (new_socket.binding() == ERROR)
            continue;
        socket[new_socket.getSocketFd()] = new_socket;
        kq.addEvent(EVFILT_READ, new_socket.getSocketFd(), NULL);
    }
    if (kq.kq_fd == -1)
    {
        // throw error
    }
}

void Server::acceptGetClientFd()
{
    int connect_fd;

    ServerSocket serversocket = clientAccept(connect_fd);
    fcntl(connect_fd, F_SETFL, O_NONBLOCK);
    kq.addEvent(connect_fd, EVFILT_READ, NULL);
    kq.addEvent(connect_fd, EVFILT_WRITE, NULL);
    kq.saved_fd[connect_fd] = "";
}

ServerSocket Server::isServerFd(uintptr_t fd)
{
    ServerSocket tem = socket.find(fd);
    if (tem != socket.end() && tem->second.getSocketType() == 1)
        return tem;
    return NULL;
}

void Server::keventProcess()
{
    serverConnect();
    if ((kq.kq_fd == kqueue()) == -1)
        std::cout << " errorororororo" << std::endl;

    while (1)
    {
        int event_num = kq.initKevent();
        if (event_num == -1)
        {
            std::cout << "errrororroroor" << std::endl;
            exit(1);
        }
        for (int i = 0; i < event_num; i++)
        {
            if (kq.event_list[i].flags == EV_ERROR)
            {
                std::cout << "errorororor" << std::endl;
                kq.disableEvent(); //에러가 발생한 fd 삭제
                continue;
            }
            switch (kq.event_list[i].filter)
            {

            case EVFILT_READ:
                ServerSocket serverSocket = isServerFd(kq.event_list[i].ident);
                if () // fd가 서버 소켓이면 클라이언트 accept
                {
                    acceptGetClientFd(socket.find(kq.event_list[i].ident,));
                }
                else
                {
                    char buffer[1024];
                    int n = 0;
                    // if (MakeResponseMsg() == ERROR); // 만들어진 response를 어떤식으로 해당 fd에 저장할지 고민이 필요
                    //     // throw error
                    n = read(kq.event_list[i].ident, buffer, 1024 - 1);
                    buffer[n] = '\0';
                    if (n > 0)
                    {
                        // BufferAppend(buffer);
                        // MakeResponse();
                    }
                    // request[ke.eventList[i].ident] = req;
                    //     ke.saved_fd[ke.eventList[i].ident] += "hello";
                    // strLen = read(ke.eventList[i].ident, buffer, 100);
                    // if (strLen > 0)
                    // {
                    //     write(1, buffer, strlen(buffer));
                    // }
                }

            case EVFILT_WRITE:
                //     if (current_fd_status == RESPONSE_SECCEED)
                //     {
                // send_data(req->_client_write, (char *)(req->_content_type).c_str(), (char *)(req->_file_name).c_str());
                // write(ke.eventList[i].ident, buffer, strlen(buffer));
                // ke.saved_fd.erase(ke.eventList[i].ident);
                // close(ke.eventList[i].ident);
                // }
            }
        }
    }
}
