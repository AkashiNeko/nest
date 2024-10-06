#include "IPv6Address.h"

#include <cstring>

namespace nest {

IPv6Address::IPv6Address(std::string_view addr) {
    if (addr.size() > 40) {
        valid_ = false;
        return;
    }

    if (addr.data()[addr.size()] == '\0') {
        valid_ = (1 == ::inet_pton(AF_INET6, addr.data(), &address_));
    } else {
        char buf[16]{};
        std::strncpy(buf, addr.data(), addr.size());
        valid_ = (1 == ::inet_pton(AF_INET6, buf, &address_));
    }
}

std::string IPv6Address::to_string() const {
    char buffer[INET6_ADDRSTRLEN];
    ::inet_ntop(AF_INET6, &address_, buffer, sizeof(buffer));
    return std::string(buffer);
}

} // namespace nest
