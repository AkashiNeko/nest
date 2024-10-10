#pragma once

#ifndef NEST_EPOLL_POLLER_H
#define NEST_EPOLL_POLLER_H

#ifdef __linux__

#include "poller/EventDispatcher.h"

namespace nest {

class EpollPoller : public EventDispatcher {
public:
    explicit EpollPoller();

    ~EpollPoller() override;

    bool add_event(socket_t sock, void* ptr) override;

    bool remove_event(socket_t sock) override;

    bool wait(const std::function<void(void*)>& f, int timeout = -1) override;

private:
    int epfd_;

}; // class EpollPoler

} // namespace nest

#endif // __linux__

#endif // NEST_EPOLL_POLLER_H
