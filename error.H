#ifndef ERROR_H_INCLUDED
#  define ERROR_H_INCLUDED

#include "fmt/format.h"

#include <stdexcept>
#include <string>

class Error : public std::exception {
public:

    inline explicit Error(std::string msg)
        : _msg(std::move(msg))
    {}

    inline virtual ~Error() noexcept {}

    inline virtual char const *what() const noexcept { return _msg.c_str(); }

protected:

    std::string _msg;
};

#define THROW_ERROR(format_str, ...)                                                                                   \
    {                                                                                                                  \
        throw Error{fmt::format(format_str, __VA_ARGS__)};                                                             \
    }

#endif
