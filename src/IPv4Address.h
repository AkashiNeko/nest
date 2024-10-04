#pragma once

#ifndef NEST_IPV4_ADDRESS_H
#define NEST_IPV4_ADDRESS_H

#include "IPAddress.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

#include <string_view>

namespace nest {

class IPv4Address : public IPAddress {
public:
    explicit IPv4Address(std::string_view addr);

    inline explicit IPv4Address(const std::string& addr)
        : IPv4Address(std::string_view(addr)) {}
    
    inline explicit IPv4Address(const char* addr)
        : IPv4Address(std::string_view(addr)) {}

    inline bool isValid() const override { return valid_; }

    inline Family family() const override { return Family::IPV4; }

    inline const in_addr& raw_in_addr() const { return address_; }

    std::string toString() const override;

private:
    struct in_addr address_;

};

inline IPv4Address operator ""_ipv4(const char* str, size_t) {
    return IPv4Address(str);
}

} // namespace nest

#endif // NEST_IPV4_ADDRESS_H
