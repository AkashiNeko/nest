#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <functional>

#include "IPAddress.h"
#include "TcpSocket.h"

#ifdef __linux__
#include "poller/EpollPoller.h"
#endif

namespace nest {

template <class UserData>
class TcpServer {
public:
    using AcceptHandler = std::function<void(Socket&, UserData&)>;
    using MessageHandler = std::function<void(Socket&, UserData&, std::string_view)>;
    using CloseHandler = std::function<void(UserData&)>;

    explicit TcpServer() : poller_(new EpollPoller) {}

    inline void on_open(AcceptHandler handler) {
        on_accept_handler_ = std::move(handler);
    }

    inline void on_message(MessageHandler handler) {
        on_message_handler_ = std::move(handler);
    }

    inline void on_closed(CloseHandler handler) {
        on_close_handler_ = std::move(handler);
    }

    inline bool listen(const IPAddress& address, port_t port) {
        return server_.listen(address, port);
    }

    inline error_t error() const {
        return server_.error();
    }

    inline const char* str_error() const {
        return server_.str_error();
    }

    void run() {
        server_.nonblock();
        poller_->add_event(server_.raw_socket(), nullptr,
            [this](socket_t sock, void*) {
                server_event_handler(sock);
            }
        );
        server_.before_close([this] () {
            poller_->remove_event(
                server_.raw_socket(), [](void*) {});
        });
        while (true) poller_->wait();
    }

private:
    void connection_event_handler(socket_t connfd, UserData& userdata) {
        TcpSocket sock(connfd);
        sock.before_close([this, connfd]() {
            poller_->remove_event(connfd, [](void* p) {
                delete reinterpret_cast<UserData*>(p);
            });
        });
        char buf[4096];
        while (true) {
            int res = sock.read(buf, sizeof(buf));
            if (res > 0) {
                if (static_cast<std::size_t>(res) != sizeof(buf))
                    buf[res] = '\0';
                this->on_message_handler_(
                    sock, userdata, std::string_view(buf, res));
            } else {
                if (res == -1 && sock.error() == EAGAIN)
                    break;
                sock.close();
                if (res == 0)
                    this->on_close_handler_(userdata);
            }
        }
    }

    void server_event_handler(socket_t servfd) {
        
        TcpSocket server(servfd);
        TcpSocket socket = server.accept();
        socket.nonblock();

        UserData* userdata = new UserData;
        socket.before_close([this, raw=socket.raw_socket()]() {
            poller_->remove_event(raw, [](void* p) {
                delete reinterpret_cast<UserData*>(p);
            });
        });
        this->on_accept_handler_(socket, *userdata);

        socket_t raw = socket.raw_socket();
        poller_->add_event(raw, userdata, [this](socket_t s, void* p) {
            connection_event_handler(s, *reinterpret_cast<UserData*>(p));
        });
    }

    EventDispatcher* poller_ = nullptr;
    TcpSocket server_;

    AcceptHandler on_accept_handler_;
    MessageHandler on_message_handler_;
    CloseHandler on_close_handler_;

};

} // namespace nest

#endif // TCP_SERVER_H
