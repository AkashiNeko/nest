#pragma once

#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include "Socket.h"

namespace nest {

#ifdef __linux__

// Linux: Epoll
using poller_t = int;

#elif defined(_WIN32)

// Windows: IOCP
using poller_t = HANDLE;

#elif defined(__APPLE__) || defined(__BSD__)

// MacOS & BSD: kqueue
using poller_t = int;

#else
#error "Unsupported platform"
#endif

class EventDispatcher {
public:
    virtual ~EventDispatcher() = default;

    virtual bool add_event(socket_t sock, void* ptr) = 0;

    virtual bool remove_event(socket_t sock) = 0;

    virtual bool wait(const std::function<void(void*)>& f, int timeout = -1) = 0;

}; // class EventDispatcher

} // namespace nest

#endif // EVENT_DISPATCHER_H
