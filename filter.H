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

    inline auto args() const -> auto const & { return _args; }

    auto match_dir(std::string const &name) const -> bool;
    auto exclude_dir(std::string const &name) const -> bool;
    auto match_file(std::string const &name) const -> bool;
    auto has_content_filters() const -> bool;
    auto has_exclude_content_filters() const -> bool;
    auto print_content() const -> bool;
    auto match_content(std::string_view line, Match *pmatch = nullptr) const -> bool;
    auto exclude_content(std::string_view line) const -> bool;

private:

    Args const    &_args;
    Regex::PtrList _in_content;
    Regex::PtrList _ex_content;

    static auto fnmatch(std::string const &pattern, std::string const &string, bool icase) -> bool;
};

#endif
