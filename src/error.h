#pragma once

#ifndef NEST_ERROR_H
#define NEST_ERROR_H

#include <cerrno>
#include <cstring>

namespace nest {

const error_t SUCCESS = 0;

inline error_t net_error_code() {
#ifdef __linux__
    return errno;
#endif
}

inline const char* net_error_str(error_t code = errno) {
#ifdef __linux__
    return std::strerror(code);
#endif
}

}

#endif // NEST_ERROR_H
