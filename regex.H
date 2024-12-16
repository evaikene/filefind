#ifndef RE2_REGEX_H_INCLUDED
#  define RE2_REGEX_H_INCLUDED

#include <memory>
#include <string_view>
#include <vector>

namespace re2 {
class RE2;
}

class ArgVal;

class Match {
public:

    /// Ctor
    Match() = default;

    /// Dtor
    ~Match() = default;

    inline auto position() const { return _position; }

    inline auto length() const { return _length; }

    inline void set_pos_and_len(size_t pos, size_t len)
    {
        _position = pos;
        _length   = len;
    }

private:

    size_t _position = 0;
    size_t _length   = 0;
};

class Regex {
public:

    using Ptr     = std::unique_ptr<Regex>;
    using PtrList = std::vector<Ptr>;

    explicit Regex(ArgVal const &r);
    ~Regex();

    inline auto valid() const { return _valid; }

    auto match(std::string_view s, Match *pmatch = nullptr) const -> bool;

private:

    bool                      _valid = false;
    std::unique_ptr<re2::RE2> _rx;
};

#endif
