#pragma once

#ifndef NEST_SOCKET_H
#define NEST_SOCKET_H

#ifdef __linux__
#include <sys/socket.h>
#endif

#include "IPAddress.h"
#include "Port.h"

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

namespace nest {

using socket_t = int;

class Socket {
public:

    Socket() = default;
    virtual ~Socket() = default;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    explicit Socket(Socket&&);
    Socket& operator=(Socket&&);

    inline bool isOpen() const { return socket_ != INVALID_SOCKET; }
    inline socket_t raw_socket() const { return socket_; }

    virtual bool bind(const IPAddress& address, Port port) = 0;
    virtual bool connect(const IPAddress& address, Port port) = 0;
    virtual void close() = 0;

protected:
    socket_t socket_ = INVALID_SOCKET;

};

} // namespace nest

#endif // NEST_SOCKET_H
