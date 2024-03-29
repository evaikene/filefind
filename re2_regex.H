#ifndef RE2_REGEX_H
#define RE2_REGEX_H

#include <string>
#include <list>
#include <memory>

namespace re2 {
    class RE2;
}

class String;

class Match {
public:

    /// Ctor
    Match() = default;

    /// Dtor
    ~Match() = default;

    inline size_t position() const
    {
        return _position;
    }

    inline size_t length() const
    {
        return _length;
    }

    inline void set_pos_and_len(size_t pos, size_t len)
    {
        _position = pos;
        _length = len;
    }


private:

    size_t _position = 0;
    size_t _length = 0;

};

class Regex {
public:

    using Ptr = std::unique_ptr<Regex>;
    using PtrList = std::list<Ptr>;

    explicit Regex(String const & r, std::string const & grammar);
    ~Regex();

    inline bool valid() const { return _valid; }
    bool match(std::string const & s, Match * pmatch = nullptr) const;

private:

    bool _valid = false;
    std::unique_ptr<re2::RE2> _rx;
};

#endif
