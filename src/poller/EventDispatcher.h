#pragma once

#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include "Socket.h"
#include <memory>

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

    virtual bool add_event(socket_t sock,
                           void* userdata,
                           const std::function<void(socket_t, void*)> handler) = 0;

    virtual bool remove_event(socket_t sock,
                              const std::function<void(void*)>& handler) = 0;

    virtual void wait(int /* milliseconds */ timeout = -1) = 0;

protected:
    struct EventData {
        socket_t socket;
        void* userdata;
        std::function<void(socket_t, void*)> handler;
    }; // struct EventData

    std::unordered_map<socket_t, std::shared_ptr<EventData>> map_;

}; // class EventDispatcher

} // namespace nest

#endif // EVENT_DISPATCHER_H
