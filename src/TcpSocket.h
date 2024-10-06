#pragma once

#ifndef NEST_TCP_SOCKET_H
#define NEST_TCP_SOCKET_H

#include "Socket.h"

namespace nest {

class TcpSocket : public Socket {
public:
    explicit TcpSocket() = default;
    explicit TcpSocket(socket_t sock);
    ~TcpSocket() = default;

    bool connect(const IPAddress& address, port_t port = 0U);

    bool bind(const IPAddress& address, port_t port = 0U) override;
    int read(char* buffer, std::size_t size) const override;
    int write(const char* buffer, std::size_t size) const override;
    bool listen(const IPAddress& address, port_t port);
    TcpSocket accept() const;

}; // class TcpSocket

} // namespace nest

#endif // NEST_TCP_SOCKET_H
