#include "utils.h"

#include "IPv4Address.h"
#include "IPv6Address.h"

namespace nest::utils {

std::pair<sockaddr*, socklen_t> make_sockaddr(const IPAddress& address, port_t port) {
    static thread_local sockaddr_in in = { .sin_family = AF_INET };
    static thread_local sockaddr_in6 in6 = { .sin6_family = AF_INET6 };
    if (address.family() == IPV4) {
        in.sin_port = htons(port);
        in.sin_addr = dynamic_cast<const IPv4Address&>(address).raw_inaddr();
        return { reinterpret_cast<sockaddr*>(&in), sizeof(sockaddr_in) };
    } else /* if (address.family() == IPV6) */ {
        in6.sin6_port = htons(port);
        in6.sin6_addr = dynamic_cast<const IPv6Address&>(address).raw_in6addr();
        return { reinterpret_cast<sockaddr*>(&in6), sizeof(sockaddr_in6) };
    }
}

} // namespace nest::utils
