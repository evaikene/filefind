#ifndef __FILTER_H
#define __FILTER_H

#include <boost/shared_ptr.hpp>

#include <string>
#include <list>

#include <regex.h>

class Args;
class Regex;

class Filter
{
public:

    explicit Filter(Args const & args);
    inline ~Filter()
    {}
    inline Args const & args() const
    {
        return m_args;
    }

    bool matchDir(std::string const & name) const;
    bool excludeDir(std::string const & name) const;
    bool matchFile(std::string const & name) const;
    bool hasContentFilters() const;
    bool hasExcludeContentFilters() const;
    bool printContent() const;
    bool matchContent(std::string const & line, regmatch_t * pmatch = NULL) const;
    bool excludeContent(std::string const & line) const;

private:

    Args const & m_args;
    std::list<boost::shared_ptr<Regex> > m_inContent;
    std::list<boost::shared_ptr<Regex> > m_exContent;
};

#endif

