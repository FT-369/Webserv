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
	std::map<int, std::string> saved_fd;
	int kq_fd;
public :
	KqueueHandler();
	int	initKevent();
	void addEvent(int16_t filter, uintptr_t ident, void *udata);
	void enableEvent(int16_t filter, uintptr_t ident, void *udata);
	void disableEvent(int16_t filter, uintptr_t ident, void *udata);
	void removeEvent(int16_t filter, uintptr_t ident, void *udata);
	int isServerFd();
};

#endif
