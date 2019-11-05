#ifndef __REGEX_H
#define __REGEX_H

#include <string>
#include <list>
#include <memory>

#include <regex.h>

class String;

class Regex
{
public:
    
    typedef std::unique_ptr<Regex> Ptr;
    typedef std::list<Ptr> PtrList;

    Regex(String const & r);
    ~Regex();

    bool valid() const { return m_valid; }
    bool match(std::string const & s, regmatch_t * pmatch = nullptr) const;

private:

    bool m_valid;
    regex_t m_preg;
};

#endif // REGEX_H

