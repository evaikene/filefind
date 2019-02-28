#ifndef __ERROR_H
#define __ERROR_H

#include <stdexcept>
#include <string>

#include <stdio.h>

class Error : public std::exception
{
public:

    inline explicit Error(std::string const & msg)
        : m_msg(msg)
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

#define ERROR(fmt, ...) \
{ \
    char * buf = NULL; \
    if (asprintf(&buf, fmt, __VA_ARGS__) == -1) \
    { \
        throw std::runtime_error("asprintf returned -1"); \
    } \
    std::string const msg(buf); \
    free(buf); \
    throw Error(msg); \
}

#endif

