#include "Server.hpp"
#include "Request.hpp"
#include <stdio.h>
#include <sys/event.h>

Server::Server() {}

Server::Server(Config &con) : config(con) {}

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
        ServerSocket *new_socket = new ServerSocket(servers[i]);
        std::cout << "server host:" << new_socket->getSocketHost() << std::endl;
        std::cout << "server port:" << new_socket->getSocketPort() << std::endl;

        if (new_socket->binding() == ERROR)
            continue;
        socket[new_socket->getSocketFd()] = new_socket;
        kq.addEvent(EVFILT_READ, new_socket->getSocketFd(), NULL);
    }
    std::cout << "map_socket_size: " << socket.size() << std::endl;
}

void Server::acceptGetClientFd(ServerSocket *server_socket)
{
    int connect_fd;

    server_socket->clientAccept(connect_fd);
    fcntl(connect_fd, F_SETFL, O_NONBLOCK);
    ClientSocket *new_socket = new ClientSocket(connect_fd, server_socket->getServerInfo());
    socket[new_socket->getSocketFd()] = new_socket;
    std::cout << "new_socket = " << new_socket->getSocketFd() << std::endl;
    kq.addEvent(EVFILT_READ, connect_fd, NULL);
    kq.addEvent(EVFILT_WRITE, connect_fd, NULL);
    // kq.saved_fd[connect_fd] = "";
}

ServerSocket *Server::isServerFd(uintptr_t fd)
{
    if (socket[fd]->getSocketType() == SERVER_SOCKET)
    {
        return dynamic_cast<ServerSocket *>(socket[fd]);
    }
    return NULL;
}

void send_data(Request *request)
{
    std::cout << "send data!" << std::endl;

    FILE *fp = fdopen(dup(request->socket_fd), "w");
	FILE *send_file = fopen("./static_file/index.html", "r");

    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server: Mac Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[] = "Content-type:text/html\r\n\r\n";

    // 헤더 정보 전송
    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);

	if (send_file == NULL) {
        char html[] =
        "<!DOCTYPE html> \
        <html> \
        <head> \
        <title>Welcome to 42 Webserv!</title> \
        <style> \
        html { color-scheme: light dark; } \
        body { width: 35em; margin: 0 auto; \
        font-family: Tahoma, Verdana, Arial, sans-serif; } \
        </style> \
        </head> \
        <body> \
        <h1>Welcome to 42 Webserv!</h1> \
        <p>If you see this page, the 42 web server is successfully installed and \
        working. Further configuration is required.</p> \
        <p><em>Thank you for using 42 Webserv.</em></p> \
        </body> \
        </html>";

		// 데이터 전송
		fputs(html, fp);
    } else {
		char buf[1024];

		while (fgets(buf, 1024, send_file) != NULL)
		{
			fputs(buf, fp);
			fflush(fp);
		}
	}

    fflush(fp);
    fclose(fp);
}

void Server::keventProcess()
{
    // std::map<int, Request *> request_map;
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
        for (int i = 0; i < event_num; i++)
        {
            if (kq.event_list[i].flags == EV_ERROR)
            {
                std::cout << "errorororor" << std::endl;
                kq.disableEvent(kq.event_list[i].filter, EVFILT_WRITE, NULL); //에러가 발생한 fd 삭제
                socket.erase(kq.event_list[i].ident);
                continue;
            }
            switch (kq.event_list[i].filter)
            {
            case EVFILT_READ:
            {
                std::map<uintptr_t, Socket *>::iterator it = socket.find(kq.event_list[i].ident);
                if (it == socket.end()) // Server의 socket 리스트에 저장되지 않은 fd면 넘기기
                    continue;
                ServerSocket *server_socket = isServerFd(kq.event_list[i].ident);
                std::cout << server_socket << std::endl;
                if (server_socket) // fd가 서버 소켓이면 클라이언트 accept
                {
                    acceptGetClientFd(server_socket);
                }
                else
                {
                    std::cout << "EVFILT_READ" << std::endl;
                    ClientSocket *cs = dynamic_cast<ClientSocket *>(socket[kq.event_list[i].ident]);

                    if (cs->getRequest() == 0)
                    {
                        std::cerr << "Request() error" << std::endl;
                        continue;
                    }
					while (cs->getRequestStatus() != READ_END_OF_REQUEST) {
						if (cs->recieveRequest() == ERROR) {
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
				std::map<uintptr_t, Socket *>::iterator it = socket.find(kq.event_list[i].ident);
                if (it == socket.end()) // Server의 socket 리스트에 저장되지 않은 fd면 넘기기
                    continue;
				ClientSocket *cs = dynamic_cast<ClientSocket *>(socket[kq.event_list[i].ident]);
                if (cs != 0 && cs->getRequest() != 0 && cs->getRequestStatus() == READ_END_OF_REQUEST)
                {
                    std::cout << "EVFILT_WRITE" << std::endl;
					// send_data(cs->getRequest());

					std::vector<ConfigLocation> routes = cs->getConnectServerInfo().getLocations();

					FILE *fp = fdopen(dup(cs->getRequest()->getSocketFD()), "w");
					cs->setResponse(new Response(config.getMimeTypes(), cs->getRequest(), routes));
					fputs(cs->getResponse()->makeResponse().c_str(), fp);
					fflush(fp);
					fclose(fp);
					std::cout << "EVFILT_WRITE - fd[" << kq.event_list[i].ident << "]: " << cs << std::endl;
					socket.erase(kq.event_list[i].ident);
					close(kq.event_list[i].ident);
                }
            }
            }
        }
    }
}
