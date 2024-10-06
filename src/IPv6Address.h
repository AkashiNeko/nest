#pragma once

#ifndef NEST_IPV6_ADDRESS_H
#define NEST_IPV6_ADDRESS_H

#include "IPAddress.h"

#include <netinet/in.h>
#include <arpa/inet.h>

#include <string_view>

namespace nest {

class IPv6Address : public IPAddress {
public:
    explicit IPv6Address(std::string_view addr);

    inline explicit IPv6Address(const std::string& addr)
        : IPv6Address(std::string_view(addr)) {}
    
    inline explicit IPv6Address(const char* addr)
        : IPv6Address(std::string_view(addr)) {}

    inline bool is_valid() const override { return valid_; }
    inline Family family() const override { return Family::IPV6; }
    inline const in6_addr& raw_in6addr() const { return address_; }

    std::string to_string() const override;

private:
    struct in6_addr address_;

};

inline IPv6Address operator ""_ipv6(const char* str, size_t) {
    return IPv6Address(str);
}

} // namespace nest

#endif // NEST_IPV6_ADDRESS_H
