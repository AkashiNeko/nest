#include "TcpSocket.h"

#include "IPAddress.h"
#include "Socket.h"
#include "error.h"
#include "utils.h"

#ifdef __linux__
#include <unistd.h>
#endif

namespace nest {

namespace {

inline socket_t make_socket(Family family) {
    return ::socket((family == IPV4) ? AF_INET : AF_INET6, SOCK_STREAM, 0);
}

} // anonymous namespace

TcpSocket::TcpSocket(socket_t sock) {
    socket_ = sock;
}

bool TcpSocket::connect(const IPAddress& address, port_t port) {
    if (socket_ == INVALID_SOCKET) {
        socket_ = make_socket(address.family());
        if (socket_ == INVALID_SOCKET) {
            errno_ = net_error_code();
            return false;
        }
    }

    auto [sa, len] = utils::make_sockaddr(address, port);

    if (0 == ::connect(socket_, sa, len)) {
        errno_ = SUCCESS;
        return true;
    } else {
        errno_ = net_error_code();
        return false;
    }
}

bool TcpSocket::bind(const IPAddress& address, port_t port) {
    if (socket_ == INVALID_SOCKET) {
        socket_ = make_socket(address.family());
        if (socket_ == INVALID_SOCKET) {
            errno_ = net_error_code();
            return false;
        }
    }

    int opt = 1;
    setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    auto [sa, len] = utils::make_sockaddr(address, port);

    if (0 == ::bind(socket_, sa, len)) {
        errno_ = SUCCESS;
        return true;
    } else {
        errno_ = net_error_code();
        return false;
    }
}

int TcpSocket::read(char* buffer, std::size_t size) const {
    int res = ::recv(socket_, buffer, size, 0);
    errno_ = (res == -1) ? net_error_code() : SUCCESS;
    return res;
}

int TcpSocket::write(const char* buffer, std::size_t size) const {
    int res = ::send(socket_, buffer, size, 0);
    errno_ = (res == -1) ? net_error_code() : SUCCESS;
    return res;
}

bool TcpSocket::listen(const IPAddress& address, port_t port) {
    if (!this->bind(address, port)) return false;
    if (0 == ::listen(socket_, 20)) {
        errno_ = SUCCESS;
        return true;
    } else {
        errno_ = net_error_code();
        return false;
    }
}

TcpSocket TcpSocket::accept() const {
    socket_t sock = ::accept(socket_, nullptr, nullptr);
    if (sock != INVALID_SOCKET) {
        errno_ = SUCCESS;
        return TcpSocket{sock};
    } else {
        errno_ = net_error_code();
        return TcpSocket{INVALID_SOCKET};
    }
}

} // namespace nest
