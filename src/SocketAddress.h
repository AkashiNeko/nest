#pragma once

#ifndef NEST_SOCKET_ADDRESS_H
#define NEST_SOCKET_ADDRESS_H

#include "IPAddress.h"
#include "Port.h"

namespace nest {

class SocketAddress {
public:
    explicit SocketAddress(const IPAddress& addr, port_t port);

    ~SocketAddress();

    inline bool is_valid() const { return address_->is_valid(); }
    inline Family family() const { return address_->family(); }

    inline const IPAddress& address() const { return *address_; }
    inline port_t port() const { return port_; }
    inline void set_address(const IPAddress& addr) { *address_ = addr; }
    inline void set_port(port_t port) { port_ = port; }

    std::string to_string() const;

private:
    IPAddress* address_;
    port_t port_;

};

} // namespace nest

#endif // NEST_SOCKET_ADDRESS_H
