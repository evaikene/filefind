#ifndef REGEX_H
#define REGEX_H

#include <string>
#include <list>
#include <memory>

#include <regex>

class String;

class Regex
{
public:
    
    static std::regex::flag_type grammarFromString(std::string const & grammar);
    
    typedef std::unique_ptr<Regex> Ptr;
    typedef std::list<Ptr> PtrList;

    explicit Regex(String const & r, std::regex::flag_type flags);
    ~Regex();

    inline bool valid() const { return _valid; }
    bool match(std::string const & s, std::smatch * pmatch = nullptr) const;

private:

    bool _valid;
    std::regex _preg;
};

#endif // REGEX_H

