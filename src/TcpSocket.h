#pragma once

#ifndef NEST_TCP_SOCKET_H
#define NEST_TCP_SOCKET_H

#include "Socket.h"

namespace nest {

class TcpSocket : public Socket {
public:
    explicit TcpSocket() = default;
    ~TcpSocket() = default;

    explicit TcpSocket(socket_t sock) : Socket(sock) {}

    Result<bool> connect(const IPAddress& address, port_t port = 0U);
    Result<bool> listen(const IPAddress& address, port_t port);
    Result<TcpSocket> accept() const;

    Result<bool> bind(const IPAddress& address, port_t port = 0U) override;
    Result<int> read(char* buffer, std::size_t size) const override;
    Result<int> write(const char* buffer, std::size_t size) const override;


}; // class TcpSocket

} // namespace nest

#endif // NEST_TCP_SOCKET_H
