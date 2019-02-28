#ifndef __REGEX_H
#define __REGEX_H

#include <string>

#include <regex.h>

class String;

class Regex
{
public:

    Regex(String const & r);
    ~Regex();

    bool valid() const { return m_valid; }
    bool match(std::string const & s, regmatch_t * pmatch = nullptr) const;

private:

    bool m_valid;
    regex_t m_preg;
};

#endif // REGEX_H

