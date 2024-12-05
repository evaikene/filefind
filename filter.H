#ifndef FILTER_H_INCLUDED
#  define FILTER_H_INCLUDED

#include <string>
#include <string_view>

#include "regex.H"

class Args;

class Filter {
public:

    explicit Filter(Args const &args);
    ~Filter();

    inline Args const &args() const { return _args; }

    bool match_dir(std::string const &name) const;
    bool exclude_dir(std::string const &name) const;
    bool match_file(std::string const &name) const;
    bool has_content_filters() const;
    bool has_exclude_content_filters() const;
    bool print_content() const;
    bool match_content(std::string_view line, Match *pmatch = nullptr) const;
    bool exclude_content(std::string_view line) const;

private:

    Args const    &_args;
    Regex::PtrList _in_content;
    Regex::PtrList _ex_content;

    static bool fnmatch(std::string const &pattern, std::string const &string, bool icase);
};

#endif
