#pragma once

#ifndef NEST_TCP_SOCKET_H
#define NEST_TCP_SOCKET_H

#include "Socket.h"

namespace nest {

class TcpSocket : public Socket {
public:
    explicit TcpSocket(Family family = IPV4);

    explicit TcpSocket(const IPAddress& address, Port port = 0U);

    ~TcpSocket() override;

    bool bind(const IPAddress& address, Port port = 0U) override;

    bool connect(const IPAddress& address, Port port = 0U) override;

    bool listen();

    TcpSocket accept() const;

    void close() override;

private:
    explicit TcpSocket(socket_t sock);

}; // class TcpSocket

} // namespace nest

#endif // NEST_TCP_SOCKET_H
