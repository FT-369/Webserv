#ifndef KQUEUEHANDLER_HPP
#define KQUEUEHANDLER_HPP

#include "Config.hpp"
#include "webserv.hpp"

class KqueueHandler
{
public:
	int _n_event;
	std::vector<struct kevent> _change_list;
	struct kevent _event_list[1024];
	std::map<int, std::string> _saved_fd;
	int _kq_fd;

public:
	KqueueHandler();
	int initKevent();
	void addEvent(int16_t filter, uintptr_t ident, void *udata);
	void enableEvent(int16_t filter, uintptr_t ident, void *udata);
	void disableEvent(int16_t filter, uintptr_t ident, void *udata);
	int isServerFd();
};

#endif
