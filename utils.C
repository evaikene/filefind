#include "utils.H"

#include <algorithm>

#include <stdlib.h>
#include <string.h>

std::string Utils::strerror(int errnum)
{
    std::string rval;
#if defined(_WIN32)
    char buf[1024];
    ::strerror_s(buf, sizeof(buf), errnum);
    rval = buf;
#else
    rval = ::strerror(errnum);
#endif
    return rval;
}

std::string Utils::getenv(std::string const & name)
{
    std::string rval;
    char * buf = nullptr;
#if defined(_WIN32)
    size_t len = 0;
    errno_t err = ::_dupenv_s(&buf, &len, name.c_str());
    if (err == 0 && buf != nullptr) {
        rval = buf;
    }
    if (buf != nullptr) {
        free(buf);
    }
#else
    if ((buf = ::getenv(name.c_str())) != nullptr) {
        rval = buf;
    }
#endif
    return rval;
}

FILE * Utils::fopen(std::string const & path, std::string const & mode)
{
    FILE * rval = nullptr;
#if defined(_WIN32)
    errno_t err = ::fopen_s(&rval, path.c_str(), mode.c_str());
    if (err != 0) {
        rval = nullptr;
    }
#else
    rval = ::fopen(path.c_str(), mode.c_str());
#endif
    return rval;
}

char * Utils::strncpy_s(char * dst, size_t sz, char const * src, size_t len)
{
    size_t rlen = std::min<size_t>(sz - 1, len);
#if defined(_WIN32)
    ::strncpy_s(dst, sz, src, rlen);
    return dst;
#else
    char * rval = ::strncpy(dst, src, rlen);
    dst[rlen] = '\0';
    return rval;
#endif
}
