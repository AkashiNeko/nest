#pragma once

#ifndef NEST_PORT_H
#define NEST_PORT_H

#include <cstdint>

namespace nest {

using port_t = std::uint16_t;

class Port {
public:
    Port(port_t port = 0) : port_(port) {}

    inline operator unsigned short() const { return port_; }

private:
    port_t port_;
};

} // namespace nest

#endif // NEST_PORT_H
