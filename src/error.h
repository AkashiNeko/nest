#pragma once

#ifndef NEST_ERROR_H
#define NEST_ERROR_H

#include <cerrno>
#include <cstring>
#include <utility>

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

template <typename T>
struct Result {
public:
    Result(T&& v, error_t e = 0)
        : value(std::move(v)), error(e) {}

    Result(const T& v, error_t e = 0)
        : value(v), error(e) {}

    T value;
    error_t error;
}; // struct Result

} // namespace nest

#endif // NEST_ERROR_H
