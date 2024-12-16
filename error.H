#ifndef ERROR_H_INCLUDED
#  define ERROR_H_INCLUDED

#include "fmt/format.h"

#include <stdexcept>
#include <string>

class Error : public std::runtime_error {
public:

    inline explicit Error(std::string const &msg)
        : std::runtime_error(msg)
    {}

    template <typename... Args>
    Error(std::string const &fmt, Args const &...args)
        : std::runtime_error(fmt::format(fmt, args...))
    {}
};

#endif
