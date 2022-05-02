#ifndef KQUEUEHANDLER_HPP
#define KQUEUEHANDLER_HPP

#include "Config.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>

class KqueueHandler
{
public :
	int n_event;
	std::vector<struct kevent> change_list;
	struct kevent event_list[10];
	std::vector<int> servers_fd;
	std::map<int, std::string> saved_fd;
	int kq_fd;
public :
	KqueueHandler();
	void changeEvent(std::vector<struct kevent> &k, uintptr_t ident,
                    int16_t filter, intptr_t data, void *udata);
	int	initKevent();
	int disableEvent();
	int isServerFd();
};

#endif
