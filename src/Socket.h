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

    explicit Socket(socket_t sock) : socket_(sock) {}

    Socket(const Socket&) = default;
    Socket(Socket&&) = default;

    Socket& operator=(const Socket&);
    Socket& operator=(Socket&&);

    inline bool is_open() const { return socket_ != INVALID_SOCKET; }
    inline socket_t raw_socket() const { return socket_; }

    void nonblock(bool value = true);

    virtual Result<bool> bind(const IPAddress& address, port_t port) = 0;
    virtual Result<int> read(char* buffer, std::size_t size) const = 0;
    virtual Result<int> write(const char* buffer, std::size_t size) const = 0;

    virtual void close();

protected:
    socket_t socket_ = INVALID_SOCKET;

};

} // namespace nest

#endif // NEST_SOCKET_H
