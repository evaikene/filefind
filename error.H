#ifndef __ERROR_H
#define __ERROR_H

#include <stdexcept>
#include <string>

#include <stdio.h>

#if defined(_AIX)
#include <stdarg.h>
inline int asprintf(char ** ret, char const * format, ...)
{
    *ret = nullptr;
    va_list ap;

    va_start(ap, format);
    int cnt = vsnprintf(nullptr, 0, format, ap);
    va_end(ap);
    if (cnt > 0) {
        char * buf = reinterpret_cast<char *>(malloc(cnt + 1));
        if (buf == nullptr) {
            return -1;
        }

        va_start(ap, format);
        cnt = vsnprintf(buf, cnt + 1, format, ap);
        va_end(ap);

        if (cnt < 0) {
            free(buf);
            return cnt;
        }
        *ret = buf;
    }
    return cnt;
}
#endif

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

