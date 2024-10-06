#pragma once

#ifndef NEST_SOCKET_H
#define NEST_SOCKET_H

#ifdef __linux__
#include <sys/socket.h>
#endif

#include <functional>
#include <cstring>

#include "IPAddress.h"
#include "Port.h"
#include "error.h"

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

namespace nest {

using socket_t = int;

class Socket {
public:

    explicit Socket() = default;
    virtual ~Socket() = default;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    explicit Socket(Socket&&);
    Socket& operator=(Socket&&);

    inline bool is_open() const { return socket_ != INVALID_SOCKET; }
    inline int error() const { return errno_; }
    inline const char* str_error() const { return net_error_str(errno_); }
    inline socket_t raw_socket() const { return socket_; }

    virtual bool bind(const IPAddress& address, port_t port) = 0;
    virtual int read(char* buffer, std::size_t size) const = 0;
    virtual int write(const char* buffer, std::size_t size) const = 0;

    void nonblock(bool value = true);

    void close();

    inline void before_close(std::function<void()> handler) {
        on_before_close_ = handler;
    }

protected:
    socket_t socket_ = INVALID_SOCKET;
    mutable error_t errno_ = SUCCESS;

private:
    std::function<void()> on_before_close_;

};

} // namespace nest

#endif // NEST_SOCKET_H
