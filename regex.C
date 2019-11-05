#include "regex.H"
#include "args.H"
#include "error.H"

#include <stddef.h>
#include <string.h>

Regex::Regex(String const & r)
    : m_valid(false)
{
    int const rval = regcomp(&m_preg, r.c_str(), REG_EXTENDED + (r.noCase() ? REG_ICASE : 0));
    m_valid = (rval == 0);
    if (!m_valid) {
        char buf[256];
        regerror(rval, &m_preg, buf, sizeof(buf));
        throw Error(std::string(buf));
    }
}

Regex::~Regex()
{
    if (m_valid) {
        regfree(&m_preg);
    }
}

bool Regex::match(std::string const & s, regmatch_t * pmatch) const
{
    bool rval = false;
    if (m_valid) {
        size_t nmatch = 0;
        if (pmatch != nullptr) {
            nmatch = 1;
#if defined(_AIX)
	    memset(pmatch, 0, sizeof(regmatch_t));
#else
            bzero(pmatch, sizeof(regmatch_t));
#endif
        }
        rval = (regexec(&m_preg, s.c_str(), nmatch, pmatch, 0) == 0);
    }
    return rval;
}
