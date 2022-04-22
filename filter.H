#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <list>

#include "regex.H"

class Args;

class Filter
{
public:

    explicit Filter(Args const & args);
    ~Filter();
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
    bool matchContent(std::string const & line, Match * pmatch = nullptr) const;
    bool excludeContent(std::string const & line) const;

private:

    Args const & m_args;
    Regex::PtrList m_inContent;
    Regex::PtrList m_exContent;

	static bool fnmatch(std::string const& pattern, std::string const& string, bool icase);
};

#endif
