#pragma once

#ifndef NEST_UTILS_H
#define NEST_UTILS_H

#ifdef __linux__
#include <sys/socket.h>
#endif

#include "IPAddress.h"
#include "Port.h"

namespace nest {

std::pair<sockaddr*, socklen_t> make_sockaddr(const IPAddress& address, port_t port);

} // namespace nest::utils

#endif // NEST_UTILS_H
