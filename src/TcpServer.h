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
class TcpServer {
private:
    friend class TcpSocketOverrideClose;

    class TcpSocketOverrideClose : public TcpSocket {
    public:
        inline TcpSocketOverrideClose(socket_t raw, TcpServer* server)
            : TcpSocket{raw}, server_(server) {}

        void close() override {
            server_->poller_->remove_event(socket_);
            server_->map_.erase(socket_);
            TcpSocket::close();
        }

    private:
        TcpServer* server_;
    }; // class TcpServer::TcpSocketOverrideClose

    struct TcpServerData {
        std::mutex mutex;
        std::unique_ptr<UserData> userdata;
    }; // struct TcpServer::TcpServerData

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
        return server_.listen(address, port);
    }

    bool run(std::size_t threads = 4) {
        if (!server_.is_open()) return false;
        server_.nonblock();
        poller_->add_event(server_.raw_socket());
        tp_.run(threads);

        bool result = false;
        while (!stop_) result = poller_->wait(
            [this](socket_t sock) {
                if (sock == server_.raw_socket()) {
                    on_server_event(sock);
                } else if (const auto it = map_.find(sock);
                           it != map_.end() && it->second->mutex.try_lock()) {
                    on_connection_event(sock, it->second);
                }
            }
        );
        tp_.stop();
        return result;
    }

private:
    inline void on_connection_event(socket_t connfd, std::shared_ptr<TcpServerData> data) {
        tp_.push([this, data, sock = TcpSocketOverrideClose(connfd, this)]() mutable {
            char buf[4096];
            while (true) {
                auto [res, err] = sock.read(buf, sizeof(buf));
                if (res > 0) {
                    if (static_cast<std::size_t>(res) != sizeof(buf))
                        buf[res] = '\0';
                    on_message_(sock, *data->userdata.get(),
                        std::string_view(buf, res));
                } else {
                    if (err != EAGAIN) {
                        sock.close();
                        on_close_(*data->userdata.get());
                    }
                    break;
                }
            }
            data->mutex.unlock();
        });
    }

    inline void on_server_event(socket_t servfd) {
        tp_.push([this, servfd]() {
            TcpSocket server(servfd);
            while (true) {
                socket_t raw = server.accept().value.raw_socket();
                TcpSocketOverrideClose socket(raw, this);
                if (!socket.is_open()) return;
                socket.nonblock();

                auto data = new TcpServerData {
                    .userdata = std::make_unique<UserData>(),
                };
                map_.emplace(raw, data);

                this->on_open_(socket, *data->userdata.get());

                poller_->add_event(raw);
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

    std::unordered_map<socket_t, std::shared_ptr<TcpServerData>> map_;

    bool stop_ = false;

}; // class TcpServer

} // namespace nest

#endif // TCP_SERVER_H
