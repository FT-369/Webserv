#ifndef KQUEUEHANDLER_HPP
#define KQUEUEHANDLER_HPP

#include "Config.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>

class KqueueHandler
{
public:
	int _n_event;
	std::vector<struct kevent> _change_list;
	struct kevent _event_list[10];
	std::map<int, std::string> _saved_fd;
	int _kq_fd;

public:
	KqueueHandler();
	int initKevent();
	void addEvent(int16_t filter, uintptr_t ident, void *udata);
	void enableEvent(int16_t filter, uintptr_t ident, void *udata);
	void disableEvent(int16_t filter, uintptr_t ident, void *udata);
	void removeEvent(int16_t filter, uintptr_t ident, void *udata);
	int isServerFd();
};

#endif
