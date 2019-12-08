#include "filter.H"
#include "args.H"
#include "error.H"

#if defined(_WIN32)
#include <shlwapi.h>
#endif
#if defined(_UNIX)
#include <fnmatch.h>
#endif

#if defined(_AIX)
// No FNM_CASEFOLD on AIX
#define FNM_CASEFOLD 0
#endif

Filter::Filter(Args const & args)
    : m_args(args)
{
    // Build content filters
    std::list<String>::const_iterator it = m_args.includeContent().begin();
    for (; it != m_args.includeContent().end(); ++it) {
        m_inContent.push_back(Regex::Ptr(new Regex(*it, Regex::grammarFromString(args.grammar()))));
    }
    it = m_args.excludeContent().begin();
    for (; it != m_args.excludeContent().end(); ++it) {
        m_exContent.push_back(Regex::Ptr(new Regex(*it, Regex::grammarFromString(args.grammar()))));
    }
}

Filter::~Filter()
{
    m_inContent.clear();
    m_exContent.clear();
}

bool Filter::matchDir(std::string const & name) const
{
    bool match = m_args.includeDirs().empty();

    // Include filters
    std::list<String>::const_iterator it = m_args.includeDirs().begin();
    for (; !match && it != m_args.includeDirs().end(); ++it) {
		match = fnmatch(*it, name, it->noCase());
    }

    return match;
}

bool Filter::excludeDir(std::string const & name) const
{
    bool match = false;

    // Exclude filters
    std::list<String>::const_iterator it = m_args.excludeDirs().begin();
    for (; !match && it != m_args.excludeDirs().end(); ++it) {
		match = fnmatch(*it, name, it->noCase());
	}

    return match;
}

bool Filter::matchFile(std::string const & name) const
{
    bool match = m_args.includeFiles().empty();

    // Include filters
    std::list<String>::const_iterator it = m_args.includeFiles().begin();
    for (; !match && it != m_args.includeFiles().end(); ++it) {
		match = fnmatch(*it, name, it->noCase());
    }

    // Exclude filters
    it = m_args.excludeFiles().begin();
    for (; match && it != m_args.excludeFiles().end(); ++it) {
		match = !fnmatch(*it, name, it->noCase());
    }

    return match;
}

bool Filter::hasContentFilters() const
{
    return !m_inContent.empty();
}

bool Filter::hasExcludeContentFilters() const
{
    return !m_exContent.empty();
}

bool Filter::printContent() const
{
    return !m_inContent.empty() && m_args.allContent();
}

bool Filter::matchContent(std::string const & line, std::smatch * pmatch) const
{
    bool match = m_inContent.empty();

    // Include filters
    Regex::PtrList::const_iterator it = m_inContent.begin();
    for (; !match && it != m_inContent.end(); ++it) {
        match = (*it)->match(line, pmatch);
    }
    return match;
}

bool Filter::excludeContent(std::string const & line) const
{
    bool exclude = false;

    // Exclude filters
    Regex::PtrList::const_iterator it = m_exContent.begin();
    for (; !exclude && it != m_exContent.end(); ++it) {
        exclude = (*it)->match(line);
    }
    return exclude;
}

bool Filter::fnmatch(std::string const & pattern, std::string const & string, bool icase)
{
#if defined(_UNIX)
	// POSIX fnmatch
	int const rval = ::fnmatch(pattern.c_str(), string.c_str(), icase ? FNM_CASEFOLD : 0);
	if (rval != 0 && rval != FNM_NOMATCH) {
		THROW_ERROR("Invalid pattern \"%s\" : %s", pattern.c_str(), strerror(errno));
	}
	return (rval == 0);
#endif
#if defined(_WIN32)
	return PathMatchSpecA(string.c_str(), pattern.c_str());
#endif
}
