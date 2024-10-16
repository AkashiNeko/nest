#include "Socket.h"

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#elif define(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

namespace nest {

Socket& Socket::operator=(const Socket& socket) {
    this->close();
    this->socket_ = socket.socket_;
    return *this;
}

Socket& Socket::operator=(Socket&& socket) {
    this->close();
    this->socket_ = socket.socket_;
    socket.socket_ = INVALID_SOCKET;
    return *this;
}

void Socket::nonblock(bool value) {
#ifdef __linux__
    int flags = ::fcntl(socket_, F_GETFL, 0);
    if (value) {
        ::fcntl(socket_, F_SETFL, flags | O_NONBLOCK);
    } else {
        ::fcntl(socket_, F_SETFL, flags & ~O_NONBLOCK);
    }
#elif define(_WIN32)
    u_long mode = value ? 1 : 0;
    ioctlsocket(socket_, FIONBIO, &mode);
#endif
}

void Socket::close() {
    if (socket_ != INVALID_SOCKET) {
        ::close(socket_);
        socket_ = INVALID_SOCKET;
    }
}


} // namespace nest