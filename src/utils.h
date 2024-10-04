#pragma once

#ifndef NEST_UTILS_H
#define NEST_UTILS_H

#ifdef __linux__
#include <sys/socket.h>
#endif

#include "IPAddress.h"
#include "Port.h"

namespace nest::utils {

std::pair<sockaddr*, socklen_t> make_sockaddr(const IPAddress& address, Port port);

} // namespace nest::utils

#endif // NEST_UTILS_H
