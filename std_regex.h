#ifndef STD_REGEX_H
#define STD_REGEX_H

#include <string>
#include <list>
#include <memory>

#include <regex>

class String;

class Match {
public:

    /// Ctor
    Match() = default;

    /// Dtor
    ~Match() = default;

    inline size_t position() const
    {
        return _smatch.position();
    }

    inline size_t length() const
    {
        return _smatch.length();
    }

    inline std::smatch & smatch()
    {
        return _smatch;
    }


private:

    std::smatch _smatch;
};

class Regex {
public:

    typedef std::unique_ptr<Regex> Ptr;
    typedef std::list<Ptr> PtrList;

    explicit Regex(String const & r, std::string const & grammar);
    ~Regex();

    inline bool valid() const { return _valid; }
    bool match(std::string const & s, Match * pmatch = nullptr) const;

private:

    bool _valid;
    std::regex _preg;
};

#endif // STD_REGEX_H
