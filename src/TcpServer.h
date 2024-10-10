#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <functional>
#include <mutex>
#include <memory>

#include "TcpSocket.h"
#include "poller/ThreadPool.h"
#include "error.h"

#ifdef __linux__

#include "poller/EpollPoller.h"
namespace nest {
using Poller = EpollPoller;
}

#endif

namespace nest {

template <class UserData>
class  TcpServer {
private:
    struct TcpServerData {
        socket_t socket;
        std::unique_ptr<UserData> userdata;
        std::mutex mutex;
    }; // struct TcpServer::TcpServerData

    friend class TcpSocketOverrideClose;

    class TcpSocketOverrideClose : public TcpSocket {
    public:
        inline TcpSocketOverrideClose(socket_t raw,
                                      TcpServer* server,
                                      TcpServerData* data)
            : TcpSocket{raw}, server_(server), data_(data) {}

        void close() override {
            // Override the close function: When the user closes the socket, it
            // automatically removes the socket from the poller and frees the data_.
            server_->poller_->remove_event(data_->socket);
            delete data_;
            TcpSocket::close();
        }

    private:
        TcpServer* server_;
        TcpServerData* data_;
    }; // class TcpServer::TcpSocketOverrideClose

public:
    explicit TcpServer() : poller_(new Poller) {}

    inline void on_open(std::function<void(TcpSocket&, UserData&)> f) {
        on_open_ = std::move(f);
    }

    inline void on_message(std::function<void(TcpSocket&, UserData&, std::string_view)> f) {
        on_message_ = std::move(f);
    }

    inline void on_closed(std::function<void(UserData&)> f) {
        on_close_ = std::move(f);
    }

    inline Result<bool> listen(const IPAddress& address, port_t port) {
        auto res = server_.listen(address, port);
        listen_ = res.value;
        return res;
    }

    void run(std::size_t threads = 4) {
        if (!listen_) return;
        server_.nonblock();

        // Add server socket to the poller
        poller_->add_event(server_.raw_socket(), nullptr);

        // Start thread pool
        tp_.run(threads);

        // Event loop
        while (poller_->wait([this](void* ptr) {
            auto data = reinterpret_cast<TcpServerData*>(ptr);
            if (!ptr) {
                on_server_event();
            } else {
                if (data->mutex.try_lock())
                    on_connection_event(data);
            }
        }));
    }

private:
    inline void on_server_event() {
        tp_.push([this]() {
            TcpSocket server(this->server_.raw_socket());
            while (true) {
                TcpSocket socket = server.accept().value;
                if (!socket.is_open()) return;
                socket.nonblock();

                TcpServerData* data = nullptr;
                try {
                    data = new TcpServerData {
                        .socket = socket.raw_socket(),
                        .userdata = std::make_unique<UserData>(),
                    };
                    if (on_open_) {
                        // Execute `on_open` callback
                        on_open_(socket, *data->userdata.get());
                    }
                } catch (...) {
                    socket.close();
                }
                if (!socket.is_open()) return;
                poller_->add_event(socket.raw_socket(), data);
            }
        });
    }

    inline void on_connection_event(TcpServerData* data) {
        tp_.push([this, data]() {
            TcpSocketOverrideClose sock(data->socket, this, data);
            char buf[4096];
            while (true) {
                auto [res, err] = sock.read(buf, sizeof(buf));
                if (res > 0) {
                    if (static_cast<std::size_t>(res) != sizeof(buf)) {
                        // The buffer is not full
                        buf[res] = '\0';
                    }

                    if (on_message_) {
                        // Execute `on_message` callback
                        on_message_(sock, *data->userdata.get(),
                                    std::string_view(buf, res));
                    }

                    if (!sock.is_open()) {
                        // The socket is closed by user
                        return;
                    }

                } else {
                    // sock.read return 0 | -1
                    if (err != EAGAIN) {
                        // Delete data pointer while closing the socket
                        sock.close();
                        if (on_close_) {
                            // Execute `on_close` callback
                            on_close_(*data->userdata.get());
                        }
                    } else {
                        // EAGAIN: The socket is not closed, mutex should be unlocked
                        data->mutex.unlock();
                    }
                    break;
                }
            }
        });
    }

private:
    EventDispatcher* poller_ = nullptr;
    TcpSocket server_;
    ThreadPool tp_;

    std::function<void(TcpSocket&, UserData&)> on_open_;
    std::function<void(TcpSocket&, UserData&, std::string_view)> on_message_;
    std::function<void(UserData&)> on_close_;

    bool stop_ = false;
    bool listen_ = false;

}; // class TcpServer

} // namespace nest

#endif // TCP_SERVER_H
