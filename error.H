#ifndef ERROR_H
#define ERROR_H

#include "fmt/format.h"

#include <stdexcept>
#include <string>

#include <stdio.h>

class Error : public std::exception {
public:

    inline explicit Error(std::string msg)
        : m_msg(std::move(msg))
    {}
    inline virtual ~Error() noexcept
    {}

    inline virtual char const * what() const noexcept
    {
        return m_msg.c_str();
    }

protected:

    std::string m_msg;
};

#define THROW_ERROR(format_str, ...) \
{ \
    throw Error{fmt::format(format_str, __VA_ARGS__)}; \
}

#endif
