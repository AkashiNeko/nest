#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <functional>
#include "IPAddress.h"
#include "TcpSocket.h"
#include "poller/EventDispatcher.h"

namespace nest {

class TcpServer {
public:
    using AcceptHandler = std::function<void(Socket&, void*& userdata)>;
    using MessageHandler = std::function<void(Socket&, void* userdata, std::string_view)>;

    explicit TcpServer();
    virtual ~TcpServer() = default;

    inline void on_open(AcceptHandler callback) {
        on_accept_handler_ = std::move(callback);
    }

    inline void on_message(MessageHandler callback) {
        on_message_handler_ = std::move(callback);
    }

    bool listen(const IPAddress& address, port_t port);
    inline error_t     error()     const { return server_.error(); }
    inline const char* str_error() const { return server_.str_error(); }

    void run();

private:
    EventDispatcher* poller_ = nullptr;
    TcpSocket server_;
    AcceptHandler on_accept_handler_;
    MessageHandler on_message_handler_;

    const EventDispatcher::Handler connection_event_handler_;
    const EventDispatcher::Handler server_event_handler_;

};

} // namespace nest

#endif // TCP_SERVER_H
