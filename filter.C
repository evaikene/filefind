#include "args.H"
#include "error.H"
#include "filter.H"

#include <string.h>

#if defined(FF_WIN32)
#  include <shlwapi.h>
#endif
#if defined(FF_UNIX)
#  include <fnmatch.h>
#endif

#if defined(FF_AIX)
// No FNM_CASEFOLD on AIX
#  define FNM_CASEFOLD 0
#endif

Filter::Filter(Args const &args)
    : _args(args)
{
    // Build content filters
    auto it = _args.includeContent().begin();
    for (; it != _args.includeContent().end(); ++it) {
        _in_content.push_back(Regex::Ptr(new Regex(*it)));
    }
    it = _args.excludeContent().begin();
    for (; it != _args.excludeContent().end(); ++it) {
        _ex_content.push_back(Regex::Ptr(new Regex(*it)));
    }
}

Filter::~Filter()
{
    _in_content.clear();
    _ex_content.clear();
}

bool Filter::match_dir(std::string const &name) const
{
    bool match = _args.includeDirs().empty();

    // Include filters
    auto it = _args.includeDirs().begin();
    for (; !match && it != _args.includeDirs().end(); ++it) {
        match = fnmatch(*it, name, it->noCase());
    }

    return match;
}

bool Filter::exclude_dir(std::string const &name) const
{
    bool match = false;

    // Exclude filters
    auto it = _args.excludeDirs().begin();
    for (; !match && it != _args.excludeDirs().end(); ++it) {
        match = fnmatch(*it, name, it->noCase());
    }

    return match;
}

bool Filter::match_file(std::string const &name) const
{
    bool match = _args.includeFiles().empty();

    // Include filters
    auto it = _args.includeFiles().begin();
    for (; !match && it != _args.includeFiles().end(); ++it) {
        match = fnmatch(*it, name, it->noCase());
    }

    // Exclude filters
    it = _args.excludeFiles().begin();
    for (; match && it != _args.excludeFiles().end(); ++it) {
        match = !fnmatch(*it, name, it->noCase());
    }

    return match;
}

bool Filter::has_content_filters() const
{
    return !_in_content.empty();
}

bool Filter::has_exclude_content_filters() const
{
    return !_ex_content.empty();
}

bool Filter::print_content() const
{
    return !_in_content.empty() && _args.allContent();
}

bool Filter::match_content(std::string_view line, Match *pmatch) const
{
    bool match = _in_content.empty();

    // Include filters
    Regex::PtrList::const_iterator it = _in_content.begin();
    for (; !match && it != _in_content.end(); ++it) {
        match = (*it)->match(line, pmatch);
    }
    return match;
}

bool Filter::exclude_content(std::string_view line) const
{
    bool exclude = false;

    // Exclude filters
    Regex::PtrList::const_iterator it = _ex_content.begin();
    for (; !exclude && it != _ex_content.end(); ++it) {
        exclude = (*it)->match(line);
    }
    return exclude;
}

bool Filter::fnmatch(std::string const &pattern, std::string const &string, bool icase)
{
#if defined(FF_UNIX)
    // POSIX fnmatch
    int const rval = ::fnmatch(pattern.c_str(), string.c_str(), icase ? FNM_CASEFOLD : 0);
    if (rval != 0 && rval != FNM_NOMATCH) {
        THROW_ERROR("Invalid pattern \"{}\" : {}", pattern, strerror(errno));
    }
    return (rval == 0);
#elif defined(FF_WIN32)
    return PathMatchSpecA(string.c_str(), pattern.c_str());
#else
    return false;
#endif
}
