#include "KqueueHandler.hpp"

KqueueHandler::KqueueHandler() : kq_fd(0), n_event(1024)
{

}

int KqueueHandler::initKevent()
{
    int new_event;

    new_event = kevent(kq_fd, &change_list[0], change_list.size(), event_list, n_event, 0);
    change_list.clear();
    return new_event;
}

void    KqueueHandler::changeEvent(std::vector<struct kevent> &k, uintptr_t ident, 
                                    int16_t filter, intptr_t data, void *udata)
{
    struct kevent temp_event;
    EV_SET(&temp_event, ident, filter, 0, 0, data, udata);
    k.push_back(temp_event);
};
