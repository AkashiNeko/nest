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

bool EpollPoller::add_event(socket_t sock) {
    struct epoll_event event = {
        .events = EPOLLIN | EPOLLET,
        .data = { .fd = sock, },
    };
    return 0 == ::epoll_ctl(epfd_, EPOLL_CTL_ADD, sock, &event);
}

bool EpollPoller::remove_event(socket_t sock) {
    return 0 == ::epoll_ctl(epfd_, EPOLL_CTL_DEL, sock, nullptr);
}

bool EpollPoller::wait(const std::function<void(socket_t)>& f, int timeout) {
    static thread_local struct epoll_event evs[64];
    const int res = ::epoll_wait(epfd_, evs, 64, timeout);
    if (res == -1) return false;
    for (int i = 0; i < res; ++i)
        f(evs[i].data.fd);
    return true;
}

} // namespace nest

#endif // __linux__
