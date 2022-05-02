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
        SocketController new_socket = SocketController(servers[i], SERVER_SOCKET);
        if (new_socket.binding() == ERROR)
            continue;
        socket[new_socket.getSocketFd()] = new_socket;
        kq.servers_fd.push_back(new_socket.getSocketFd()); // 서버 저장 변수를 따로 만든다. kqueue에서 이 변수만 사용해 서버 fd인지 아닌지 판별한다.
        kq.changeEvent(kq.change_list, EVFILT_READ, EV_ADD | EV_ENABLE, 0, NULL);
    }
    if (kq.kq_fd == -1)
    {
        // throw error
    }
}

void Server::acceptGetClientFd()
{
    int connect_fd;

    fcntl(connect_fd, F_SETFL, O_NONBLOCK);
    kq.changeEvent(kq.change_list, connect_fd, EVFILT_READ, EV_ADD | EV_ENABLE, NULL);
    kq.changeEvent(kq.change_list, connect_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, NULL);
    kq.saved_fd[connect_fd] = "";
}

int Server::isServerFd(uintptr_t fd)
{
    // Socket tem = socket.find(fd);
    // if (tem != socket.end())
    // {
    //     socket.secon;
    // }
    // if ((*(find(socket.begin(), socket.end(), (int)fd)).getSocketType() )== 1);
}

void Server::keventProcess()
{
    int event_num = 0;

    serverConnect();
    if ((kq.kq_fd == kqueue()) == -1)
        std::cout << " errorororororo" << std::endl;

    while (1)
    {
        kq.init_kevent();
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
                if (isServerFd(kq.event_list[i].ident)) // fd가 서버 소켓이면 클라이언트 accept
                {
                    acceptGetClientFd();
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
                if (current_fd_status == RESPONSE_SECCEED)
                {
                    // send_data(req->_client_write, (char *)(req->_content_type).c_str(), (char *)(req->_file_name).c_str());
                    // write(ke.eventList[i].ident, buffer, strlen(buffer));
                    // ke.saved_fd.erase(ke.eventList[i].ident);
                    // close(ke.eventList[i].ident);
                }
            }
        }
    }
}
