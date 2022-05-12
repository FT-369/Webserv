#include "KqueueHandler.hpp"

KqueueHandler::KqueueHandler() : _kq_fd(0), _n_event(1024)
{

}

int KqueueHandler::initKevent()
{
    int new_event;

    new_event = kevent(_kq_fd, &_change_list[0], _change_list.size(), _event_list, _n_event, 0);
    _change_list.clear();
    return new_event;
}

void KqueueHandler::addEvent(int16_t filter, uintptr_t ident, void *udata){
    struct kevent temp_event;
    EV_SET(&temp_event, ident, filter, EV_ADD | EV_EOF, 0, 0, udata);
    _change_list.push_back(temp_event);
}

void KqueueHandler::disableEvent(int16_t filter, uintptr_t ident, void *udata){
    struct kevent temp_event;
    EV_SET(&temp_event, ident, filter, EV_DISABLE, 0, 0, udata);
    _change_list.push_back(temp_event);
}

void KqueueHandler::enableEvent(int16_t filter, uintptr_t ident, void *udata){
    struct kevent temp_event;
    EV_SET(&temp_event, ident, filter, EV_ENABLE, 0, 0, udata);
    _change_list.push_back(temp_event);
}

void KqueueHandler::removeEvent(int16_t filter, uintptr_t ident, void *udata){
    struct kevent temp_event;
    EV_SET(&temp_event, ident, filter, EV_DELETE, 0, 0, udata);
    _change_list.push_back(temp_event);
}
