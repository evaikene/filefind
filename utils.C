#include "utils.H"

#include <algorithm>

#include <cstdlib>
#include <cstring>

namespace Private {

void close_file(FILE *file)
{
    if (file != nullptr) {
        ::fclose(file);
    }
}

} // namespace Private

namespace Utils {

auto strerror(int errnum) -> std::string
{
    std::string rval;
#if defined(FF_WIN32)
    char buf[1024];
    ::strerror_s(buf, sizeof(buf), errnum);
    rval = buf;
#else
    rval = ::strerror(errnum);
#endif
    return rval;
}

auto getenv(char const *name) -> std::optional<std::string>
{
    if (name == nullptr || *name == '\0') {
        return std::nullopt;
    }

    std::optional<std::string> rval;
    char                      *buf = nullptr;
#if defined(FF_WIN32)
    size_t  len = 0;
    errno_t err = ::_dupenv_s(&buf, &len, name);
    if (err == 0 && buf != nullptr) {
        rval = buf;
    }
    if (buf != nullptr) {
        free(buf);
    }
#else
    buf = ::getenv(name);
    if (buf != nullptr) {
        rval = buf;
    }
#endif
    return rval;
}

auto InvalidFilePtr() -> FilePtr
{
    return FilePtr{nullptr, &Private::close_file};
}

auto fopen(std::string const &path, std::string const &mode) -> FilePtr
{
    FILE *f = nullptr;
#if defined(FF_WIN32)
    errno_t err = ::fopen_s(&f, path.c_str(), mode.c_str());
    if (err != 0) {
        f = nullptr;
    }
#else
    f = ::fopen(path.c_str(), mode.c_str());
#endif
    return FilePtr{f, &Private::close_file};
}

auto strncpy_s(char *dst, size_t sz, char const *src, size_t len) -> char *
{
    size_t rlen = std::min<size_t>(sz - 1, len);
#if defined(FF_WIN32)
    ::strncpy_s(dst, sz, src, rlen);
    return dst;
#else
    char *rval = ::strncpy(dst, src, rlen);
    dst[rlen]  = '\0';
    return rval;
#endif
}

} // namespace Utils
