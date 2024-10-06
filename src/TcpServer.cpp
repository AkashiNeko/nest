#include "TcpServer.h"
#include "TcpSocket.h"
#include "poller/EpollPoller.h"
#include <cerrno>
#include <iostream>

namespace nest {

TcpServer::TcpServer() : poller_(new EpollPoller),
    connection_event_handler_([this] (auto connfd, auto userdata) {
        TcpSocket sock(connfd);
        char buf[4096];
        while (true) {
            int res = sock.read(buf, sizeof(buf));
            if (res > 0) {
                if (static_cast<std::size_t>(res) != sizeof(buf))
                    buf[res] = '\0';
                this->on_message_handler_(
                    sock, userdata, std::string_view(buf, res));
            } else {
                if (res == -1 && sock.error() != EAGAIN)
                    sock.close();
                break;
            }
        }
    }), server_event_handler_([this](auto servfd, auto) {
        TcpSocket server(servfd);
        TcpSocket socket = server.accept();
        socket.nonblock();
        socket.before_close(
            [sockfd = socket.raw_socket(), this] () {
                poller_->remove_event(sockfd, [](void*) {
                    // TODO: free userdata
                });
            }
        );
        void* userdata = nullptr;
        this->on_accept_handler_(socket, userdata);
        poller_->add_event(socket.raw_socket(),
            userdata, connection_event_handler_);
    }) {}

bool TcpServer::listen(const IPAddress& address, port_t port) {
    return server_.listen(address, port);
}

void TcpServer::run() {
    server_.nonblock();
    poller_->add_event(server_.raw_socket(), nullptr, server_event_handler_);
    server_.before_close([this] () {
        poller_->remove_event(server_.raw_socket(), [](void*) {});
    });
    while (true) {
        poller_->wait();
    }
}

}
