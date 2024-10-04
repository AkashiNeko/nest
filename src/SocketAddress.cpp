#include "SocketAddress.h"

#include "IPv4Address.h"
#include "IPv6Address.h"
#include "Port.h"

namespace nest {

SocketAddress::SocketAddress(const IPAddress& addr, Port port)
    : address_(nullptr), port_(port) {

    if (addr.family() == IPV4) {
        address_ = new IPv4Address(dynamic_cast<const IPv4Address&>(addr));
    } else /* if (addr.getFamily() == IPV6) */ {
        address_ = new IPv6Address(dynamic_cast<const IPv6Address&>(addr));
    }
}

SocketAddress::~SocketAddress() {
    if (address_) delete address_;
}

std::string SocketAddress::toString() const {
    if (!address_->isValid()) return {};
    if (address_->family() == IPV4) {
        return address_->toString() + ':' + std::to_string(port_);
    } else {
        return '[' + address_->toString() + ']' + ':' + std::to_string(port_);
    }
}

} // namespace nest
