#include "IPv4Address.h"

#include <cstring>

namespace nest {

IPv4Address::IPv4Address(std::string_view addr) {
    if (addr.size() > 15) {
        valid_ = false;
        return;
    }

    if (addr.data()[addr.size()] == '\0') {
        valid_ = (1 == inet_pton(AF_INET, addr.data(), &address_));
    } else {
        char buf[16]{};
        std::strncpy(buf, addr.data(), addr.size());
        valid_ = (1 == inet_pton(AF_INET, buf, &address_));
    }
}

std::string IPv4Address::toString() const {
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &address_, buffer, sizeof(buffer));
    return std::string(buffer);
}

} // namespace nest
