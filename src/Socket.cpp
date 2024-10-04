#include "Socket.h"

namespace nest {

Socket::Socket(Socket&& socket) : socket_(socket.socket_) {
    socket.socket_ = INVALID_SOCKET;
}

Socket& Socket::operator=(Socket&& socket) {
    this->close();
    this->socket_ = socket.socket_;
    socket.socket_ = INVALID_SOCKET;
    return *this;
}

} // namespace nest