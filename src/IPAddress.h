#pragma once

#ifndef NEST_IP_ADDRESS_H
#define NEST_IP_ADDRESS_H

#include <string>

namespace nest {

enum Family : bool { IPV4, IPV6 };

class IPAddress {
public:
    virtual ~IPAddress() = default;
    virtual bool isValid() const = 0;
    virtual Family family() const = 0;
    virtual std::string toString() const = 0;

protected:
    bool valid_ = false;

}; // class IPAddress

} // namespace nest

#endif // NEST_IP_ADDRESS_H
