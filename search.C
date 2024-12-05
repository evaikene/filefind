#include "args.H"
#include "search.H"
#include "utils.H"

#include <fmt/color.h>
#include <fmt/format.h>

#if defined(FF_WIN32)
#  include "search_win32.H"
#else
#  include "search_unix.H"
#endif

#include <string.h>

namespace _ {

static constexpr size_t BUF_SIZE = 4096;

} // namespace _

Search *Search::_instance = nullptr;

Search &Search::instance(Args const &args)
{
    if (nullptr == _instance) {
#if defined(FF_WIN32)
        _instance = new SearchWin32(args);
#else
        _instance = new SearchUnix(args);
#endif
    }
    return *_instance;
}

void Search::destroyInstance()
{
    if (nullptr != _instance) {
        delete _instance;
        _instance = nullptr;
    }
}

Search::Search(Args const &args)
    : _args(args)
    , _filter(args)
{}

Search::~Search()
{}

void Search::search() const
{
    // Recursively search for files
    findFiles(_args.path(), "", _filter.match_dir(""));
}

void Search::findInFile(std::string const &path) const
{
    auto f = Utils::fopen(path, "r");
    if (!f) {
        fmt::println(stderr,
                     "{} Failed to open file {} : {}",
                     fmt::styled("ERROR:", fmt::fg(fmt::color::red)),
                     path,
                     Utils::strerror(errno));
        return;
    }

    char       buf[_::BUF_SIZE];
    int        lineno       = 0;
    bool       binary       = false;
    int        linesToPrint = 0;
    bool const nocolor      = _args.noColor();
    while (fgets(buf, _::BUF_SIZE, f.get()) != nullptr) {
        // Remove trailing CR and LF characters
        size_t sz = strlen(buf);
        while (sz > 0 && (buf[sz - 1] == '\n' || buf[sz - 1] == '\r')) {
            buf[--sz] = '\0';
        }

        // Check for a binary file
        binary = !binary && !_args.ascii() && memchr(buf, 0, sz) != nullptr;

        ++lineno;
        Match pmatch;
        if (_filter.match_content(buf, &pmatch)) {
            if (_filter.print_content()) {
                if (!binary) {

                    fmt::print("{} +{} : \"", path, lineno);

                    // Print content
                    size_t idx = printMatch(buf, pmatch, nocolor);

                    // Repeat search for more matches
                    while (idx < sz && _filter.match_content(&buf[idx], &pmatch)) {
                        idx += printMatch(&buf[idx], pmatch, nocolor);
                    }

                    // The remainder of the line
                    if (idx < sz) {
                        char s3[_::BUF_SIZE];
                        Utils::strncpy_s(s3, _::BUF_SIZE, &buf[idx], _::BUF_SIZE - 1);
                        fmt::println("{}\"", s3);
                    }
                    else {
                        fmt::println("\"");
                    }

                    linesToPrint = _args.extraContent();
                }
                else {
                    // Print only file name and exit
                    fmt::println("{} : binary file matches", path);
                    break;
                }
            }
            else if (!_args.execCmd().empty()) {
                // Run the command and exit
                execCmd(_args.execCmd(), path);
                break;
            }
            else {
                // Print only file name and exit
                fmt::println("{}", path);
                break;
            }
        }

        // Print extra content
        if (linesToPrint > 0) {
            fmt::println("\t{}", buf);
            --linesToPrint;
        }
    }
}

bool Search::excludeFileByContent(std::string const &path) const
{
    bool rval = false;
    auto f    = Utils::fopen(path, "r");
    if (!f) {
        fmt::println(stderr,
                     "{} Failed to open file {} : {}",
                     fmt::styled("ERROR:", fmt::fg(fmt::color::red)),
                     path,
                     Utils::strerror(errno));
        return rval;
    }

    char buf[_::BUF_SIZE];
    while (!rval && fgets(buf, sizeof(buf), f.get()) != nullptr) {
        // Remove trailing CR and LF characters
        size_t sz = strlen(buf);
        while (sz > 0 && (buf[sz - 1] == '\n' || buf[sz - 1] == '\r')) { buf[--sz] = '\0'; }
        rval = _filter.exclude_content(buf);
    }
    return rval;
}

size_t Search::printMatch(char const *buf, Match const &pmatch, bool nocolor) const
{
    size_t const sz              = strlen(buf);
    char         s1[_::BUF_SIZE] = "";
    char         s2[_::BUF_SIZE] = "";
    size_t       msz             = std::min<size_t>(pmatch.position(), _::BUF_SIZE - 1);
    Utils::strncpy_s(s1, msz + 1, buf, msz);
    size_t idx = msz;
    if (idx < sz) {
        msz = std::min<size_t>(pmatch.length(), _::BUF_SIZE - 1);
        Utils::strncpy_s(s2, msz + 1, &buf[idx], msz);
        idx += msz;
    }

    fmt::print("{}{}", s1, fmt::styled(s2, nocolor ? fmt::fg(fmt::color{}) : fmt::fg(fmt::color::red)));

    return idx;
}
