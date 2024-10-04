#pragma once

#ifndef NEST_SOCKET_ADDRESS_H
#define NEST_SOCKET_ADDRESS_H

#include "IPAddress.h"
#include "Port.h"

namespace nest {

class SocketAddress {
public:
    explicit SocketAddress(const IPAddress& addr, Port port);

    ~SocketAddress();

    inline bool isValid() const { return address_->isValid(); }
    inline Family family() const { return address_->family(); }

    inline const IPAddress& address() const { return *address_; }
    inline Port port() const { return port_; }
    inline void setAddress(const IPAddress& addr) { *address_ = addr; }
    inline void setPort(Port port) { port_ = port; }

    std::string toString() const;

private:
    IPAddress* address_;
    Port port_;

};

} // namespace nest

#endif // NEST_SOCKET_ADDRESS_H
