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

Result<bool> TcpSocket::connect(const IPAddress& address, port_t port) {
    if (socket_ == INVALID_SOCKET) {
        socket_ = make_socket(address.family());
        if (socket_ == INVALID_SOCKET) {
            return {false, net_error_code()};
        }
    }

    auto [sa, len] = make_sockaddr(address, port);

    if (0 == ::connect(socket_, sa, len)) {
        return true;
    } else {
        return {false, net_error_code()};
    }
}

Result<bool> TcpSocket::bind(const IPAddress& address, port_t port) {
    if (socket_ == INVALID_SOCKET) {
        socket_ = make_socket(address.family());
        if (socket_ == INVALID_SOCKET) {
            return {false, net_error_code()};
        }
    }

    const int opt = 1;
    setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    const auto [sa, len] = make_sockaddr(address, port);

    if (0 == ::bind(socket_, sa, len)) {
        return true;
    } else {
        return {false, net_error_code()};
    }
}

Result<int> TcpSocket::read(char* buffer, std::size_t size) const {
    const int res = ::recv(socket_, buffer, size, 0);
    if (res == -1) {
        return {res, net_error_code()};
    } else {
        return res;
    }
}

Result<int> TcpSocket::write(const char* buffer, std::size_t size) const {
    const int res = ::send(socket_, buffer, size, 0);
    if (res == -1) {
        return {res, net_error_code()};
    } else {
        return res;
    }
}

Result<bool> TcpSocket::listen(const IPAddress& address, port_t port) {
    const auto&& result = this->bind(address, port);
    if (!result.value) return std::move(result);

    if (0 == ::listen(socket_, 20)) {
        return true;
    } else {
        return {false, net_error_code()};
    }
}

Result<TcpSocket> TcpSocket::accept() const {
    socket_t sock = ::accept(socket_, nullptr, nullptr);
    if (sock != INVALID_SOCKET) {
        return std::move(TcpSocket{sock});
    } else {
        return {std::move(TcpSocket{INVALID_SOCKET}), net_error_code()};
    }
}

} // namespace nest
