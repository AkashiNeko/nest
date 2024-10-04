#include "TcpSocket.h"

#include "IPAddress.h"
#include "Socket.h"
#include "utils.h"

#ifdef __linux__
#include <unistd.h>
#endif

namespace nest {

TcpSocket::TcpSocket(Family family) {
    if (family == IPV4) {
        socket_ = socket(AF_INET, SOCK_STREAM, 0);
    } else /* if (family == IPV6) */ {
        socket_ = socket(AF_INET6, SOCK_STREAM, 0);
    }
}

TcpSocket::TcpSocket(const IPAddress& address, Port port)
    : TcpSocket(address.family()) {

    this->bind(address, port);
}

TcpSocket::TcpSocket(socket_t sock) {
    socket_ = sock;
}

TcpSocket::~TcpSocket() {
    this->close();
}

bool TcpSocket::bind(const IPAddress& address, Port port) {
    if (socket_ == INVALID_SOCKET) return false;
    auto [sa, len] = utils::make_sockaddr(address, port);
    return 0 == ::bind(socket_, sa, len);
}

bool TcpSocket::connect(const IPAddress& address, Port port) {
    if (socket_ == INVALID_SOCKET) return false;
    auto [sa, len] = utils::make_sockaddr(address, port);
    return 0 == ::connect(socket_, sa, len);
}

bool TcpSocket::listen() {
    return 0 == ::listen(socket_, 20);
}

TcpSocket TcpSocket::accept() const {
    return TcpSocket{::accept(socket_, nullptr, nullptr)};
}

void TcpSocket::close() {
    if (socket_ != INVALID_SOCKET) {
        ::close(socket_);
        socket_ = INVALID_SOCKET;
    }
}

} // namespace nest
