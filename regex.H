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
    
    typedef std::unique_ptr<Regex> Ptr;
    typedef std::list<Ptr> PtrList;

    Regex(String const & r);
    ~Regex();

    inline bool valid() const { return _valid; }
    bool match(std::string const & s, std::smatch * pmatch = nullptr) const;

private:

    bool _valid;
    std::regex _preg;
};

#endif // REGEX_H

