#include "args.H"
#include "error.H"
#include "filter.H"

#include <cstring>

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
        _in_content.push_back(std::make_unique<Regex>(*it));
    }
    it = _args.excludeContent().begin();
    for (; it != _args.excludeContent().end(); ++it) {
        _ex_content.push_back(std::make_unique<Regex>(*it));
    }
}

Filter::~Filter()
{
    _in_content.clear();
    _ex_content.clear();
}

auto Filter::match_dir(std::string const &name) const -> bool
{
    bool match = _args.includeDirs().empty();

    // Include filters
    auto it = _args.includeDirs().begin();
    for (; !match && it != _args.includeDirs().end(); ++it) {
        match = fnmatch(it->str(), name, it->noCase());
    }

    return match;
}

auto Filter::exclude_dir(std::string const &name) const -> bool
{
    bool match = false;

    // Exclude filters
    auto it = _args.excludeDirs().begin();
    for (; !match && it != _args.excludeDirs().end(); ++it) {
        match = fnmatch(it->str(), name, it->noCase());
    }

    return match;
}

auto Filter::match_file(std::string const &name) const -> bool
{
    bool match = _args.includeFiles().empty();

    // Include filters
    auto it = _args.includeFiles().begin();
    for (; !match && it != _args.includeFiles().end(); ++it) {
        match = fnmatch(it->str(), name, it->noCase());
    }

    // Exclude filters
    it = _args.excludeFiles().begin();
    for (; match && it != _args.excludeFiles().end(); ++it) {
        match = !fnmatch(it->str(), name, it->noCase());
    }

    return match;
}

auto Filter::has_content_filters() const -> bool
{
    return !_in_content.empty();
}

auto Filter::has_exclude_content_filters() const -> bool
{
    return !_ex_content.empty();
}

auto Filter::print_content() const -> bool
{
    return !_in_content.empty() && _args.allContent();
}

auto Filter::match_content(std::string_view line, Match *pmatch) const -> bool
{
    bool match = _in_content.empty();

    // Include filters
    auto it = _in_content.begin();
    for (; !match && it != _in_content.end(); ++it) {
        match = (*it)->match(line, pmatch);
    }
    return match;
}

auto Filter::exclude_content(std::string_view line) const -> bool
{
    bool exclude = false;

    // Exclude filters
    auto it = _ex_content.begin();
    for (; !exclude && it != _ex_content.end(); ++it) {
        exclude = (*it)->match(line);
    }
    return exclude;
}

auto Filter::fnmatch(std::string const &pattern, std::string const &string, bool icase) -> bool
{
#if defined(FF_UNIX)
    // POSIX fnmatch
    int const rval = ::fnmatch(pattern.c_str(), string.c_str(), icase ? FNM_CASEFOLD : 0);
    if (rval != 0 && rval != FNM_NOMATCH) {
        throw Error{"Invalid pattern \"{}\" : {}", pattern, strerror(errno)};
    }
    return (rval == 0);
#elif defined(FF_WIN32)
    return PathMatchSpecA(string.c_str(), pattern.c_str());
#else
    return false;
#endif
}
