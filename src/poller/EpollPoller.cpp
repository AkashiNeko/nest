#include "EpollPoller.h"

#ifdef __linux__

#include <sys/epoll.h>
#include <unistd.h>

namespace nest {

EpollPoller::EpollPoller() {
    epfd_ = ::epoll_create(1);
}

EpollPoller::~EpollPoller() {
    if (epfd_ != -1) {
        ::close(epfd_);
    }
}

bool EpollPoller::add_event(socket_t sock,
                            void* userdata,
                            const std::function<void(socket_t, void*)> handler) {

    auto* event_data_ptr = new EventData {
        .socket = sock,
        .userdata = userdata,
        .handler = std::move(handler),
    };

    map_[sock].reset(event_data_ptr);

    struct epoll_event event = {
        .events = EPOLLIN | EPOLLET,
        .data = { .ptr = event_data_ptr, },
    };

    if (-1 == ::epoll_ctl(epfd_, EPOLL_CTL_ADD, sock, &event)) {
        map_.erase(sock);
        return false;
    }
    return true;
}

bool EpollPoller::remove_event(socket_t sock, const std::function<void(void*)>& handler) {
    if (-1 == ::epoll_ctl(epfd_, EPOLL_CTL_DEL, sock, nullptr))
        return false;
    const auto it = map_.find(sock);
    if(handler) handler(it->second->userdata);
    if (it != map_.end()) map_.erase(it);
    return true;
}

void EpollPoller::wait(int timeout) {
    static thread_local struct epoll_event events[64];
    const int res = ::epoll_wait(epfd_, events, 64, timeout);
    for (int i = 0; i < res; ++i) {
        const EventData* ed_ptr = reinterpret_cast<EventData*>(events[i].data.ptr);
        ed_ptr->handler(ed_ptr->socket, ed_ptr->userdata);
    }
}

} // namespace nest

#endif // __linux__
