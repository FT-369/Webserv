#include "Server.hpp"
#include "Request.hpp"
#include <stdio.h>
#include <sys/event.h>

Server::Server() {}

Server::Server(Config &con) : config(con) {
    std::cout << "create Server\n";
    std::cout << (config.getHttpDirective()).getServers().size() << std::endl;
    std::cout << (con.getHttpDirective()).getServers().size() << std::endl;
}

Server::~Server() {}

void Server::serverConnect()
{
    std::vector<ConfigServer> servers = (config.getHttpDirective()).getServers();
    int server_size = servers.size();
    std::cout << "origin_ server_size = " << (config.getHttpDirective()).getServers().size() << std::endl;
    std::cout << "server_size = " << server_size << std::endl;
    for (int i = 0; i < server_size; i++)
    {
        int error_flag;
        ServerSocket new_socket = ServerSocket(servers[i]);
        std::cout << "server host:" << new_socket.getSocketHost() << std::endl;
        std::cout << "server port:" << new_socket.getSocketPort() << std::endl;

        if (new_socket.binding() == ERROR)
            continue;
        socket[new_socket.getSocketFd()] = new_socket;
        kq.addEvent(EVFILT_READ, new_socket.getSocketFd(), NULL);
    }
    std::cout << "map_socket_size: " << socket.size() << std::endl;
}

void Server::acceptGetClientFd(ServerSocket *server_socket)
{
    int connect_fd;

    server_socket->clientAccept(connect_fd);
    fcntl(connect_fd, F_SETFL, O_NONBLOCK);
    kq.addEvent(connect_fd, EVFILT_READ, NULL);
    kq.addEvent(connect_fd, EVFILT_WRITE, NULL);
    kq.saved_fd[connect_fd] = "";
}

ServerSocket *Server::isServerFd(uintptr_t fd)
{
    std::map<uintptr_t, Socket>::iterator it = socket.find(fd);
    std::cout << "(it->second).getSocketType()" << (it->second).getSocketType() << std::endl;
    if (it != socket.end()) {
        std::cout << "it != socket.end()" << std::endl;
        if (((it->second).getSocketType() == SERVER_SOCKET))
        {
            std::cout << "(it->second).getSocketType()" << std::endl;
            return reinterpret_cast<ServerSocket *>(&(it->second));
        }
    }
    if ((it != socket.end()) && ((it->second).getSocketType() == SERVER_SOCKET))
    {
        std::cout << "success" << std::endl;
        return reinterpret_cast<ServerSocket *>(&(it->second));
    }
    return NULL;
}

std::string get_content_type(std::string file)
{
    std::string html = "text/html";
    std::string plain = "text/plain";

    std::string extension = ft_split(file, ".")[1];

    if (extension == "html" || extension == "htm")
        return html;
    else
        return plain;
}

void send_data(Request *request)
{
    FILE *fp = fdopen(dup(request->socket_fd), "w");
    std::string file_name = "../static_file" + request->path.substr(1);
    std::string content_type = "Content-type:" + get_content_type(file_name) + "\r\n\r\n";

    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server: Mac Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char *cnt_type = const_cast<char *>(content_type.c_str());
    char buf[1024];
    FILE *send_file;

    std::cout << "send data!" << std::endl;
    send_file = fopen(file_name.c_str(), "r");

    if (send_file == NULL)
    {
        // send_error(fp);
        return;
    }

    // 헤더 정보 전송
    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);

    // 요청 데이터 전송
    while (fgets(buf, 1024, send_file) != NULL)
    {
        fputs(buf, fp);
        fflush(fp);
    }
    fflush(fp);
    fclose(fp);
}

void Server::keventProcess()
{
    serverConnect();
    if ((kq.kq_fd = kqueue()) == -1)
        std::cout << " errorororororo" << std::endl;

    while (1)
    {
        int event_num = kq.initKevent();
        if (event_num == -1)
        {
            std::cout << "errrororroroor" << std::endl;
            exit(1);
        }
        std::map<int, Request *> request_map;
        std::cout << "event_num" << event_num << std::endl;
        for (int i = 0; i < event_num; i++)
        {
            if (kq.event_list[i].flags == EV_ERROR)
            {
                std::cout << "errorororor" << std::endl;
                kq.disableEvent(kq.event_list[i].filter, EVFILT_WRITE, NULL); //에러가 발생한 fd 삭제
                request_map.erase(kq.event_list[i].ident);
                continue;
            }
            switch (kq.event_list[i].filter)
            {
                case EVFILT_READ:
                {
                    ServerSocket *server_socket = isServerFd(kq.event_list[i].ident);
                    if (server_socket) // fd가 서버 소켓이면 클라이언트 accept
                    {
                        std::cout << "EVFILT_READ" << std::endl;
                        acceptGetClientFd(server_socket);
                    }
                    if (request_map.find(kq.event_list[i].ident) != request_map.end())
                    {
                        Request *req = new Request(kq.event_list[i].ident);
                        if (req == 0)
                        {
                            std::cerr << "Request() error" << std::endl;
                            continue;
                        }
                        if (req->parseRequest() == ERROR)
                        {
                            std::cerr << "parseRequest() error" << std::endl;
                            continue;
                        }
                        request_map[kq.event_list[i].ident] = req;
                        // if (MakeResponseMsg() == ERROR); // 만들어진 response를 어떤식으로 해당 fd에 저장할지 고민이 필요
                        //     throw error;
                    }
            }
                case EVFILT_WRITE:
                {
                    std::cout << "EVFILT_WRITE" << std::endl;
                    if (request_map.find(kq.event_list[i].ident) != request_map.end())
                    {
                        Request *req = request_map[kq.event_list[i].ident];
                        if (req != 0)
                        {
                            send_data(req);
                            std::cout << "EVFILT_WRITE - request_map[" << kq.event_list[i].ident << "]: " << request_map[kq.event_list[i].ident] << std::endl;
                            request_map.erase(kq.event_list[i].ident);
                            close(kq.event_list[i].ident);
                        }
                    }
                //     if (current_fd_status == RESPONSE_SECCEED)
                //     {
                // send_data(req->_client_write, (char *)(req->_content_type).c_str(), (char *)(req->_file_name).c_str());
                // write(ke.eventList[i].ident, buffer, strlen(buffer));
                // ke.saved_fd.erase(ke.eventList[i].ident);
                // close(ke.eventList[i].ident);
                // }
                // default:
                //     break;
                }
            }
        }
    }
}
