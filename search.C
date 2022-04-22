#include "search.H"
#include "args.H"
#include "regex.H"
#include "utils.H"

#if defined(_WIN32)
#include "search_win32.H"
#else
#include "search_unix.H"
#endif

#include <string.h>


namespace {
    size_t const BUF_SIZE = 1024;
}

Search * Search::_instance = nullptr;

Search & Search::instance(Args const & args)
{
    if (nullptr == _instance) {
#if defined(_WIN32)
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

void Search::fclose(FILE* f)
{
    if (nullptr != f) {
        ::fclose(f);
    }
}

Search::Search(Args const & args)
    : _args(args)
    , _filter(args)
{}

Search::~Search()
{}

void Search::search() const
{
    // Recursively search for files
    findFiles(_args.path(), "", _filter.matchDir(""));
}

void Search::findInFile(std::string const & path) const
{
    std::unique_ptr<FILE, decltype(&fclose)> f(Utils::fopen(path.c_str(), "r"), &fclose);
    if (!f) {
        fprintf(stderr, "\033[31mERROR:\033[0m Failed to open file %s : %s\n", path.c_str(), Utils::strerror(errno).c_str());
        return;
    }

    char buf[BUF_SIZE];
    int lineno = 0;
    bool binary = false;
    int linesToPrint = 0;
    bool const nocolor = _args.noColor();
    while (fgets(buf, BUF_SIZE, f.get()) != nullptr) {
        // Remove trailing CR and LF characters
        size_t sz = strlen(buf);
        while (sz > 0 && (buf[sz - 1] == '\n' || buf[sz - 1] == '\r')) {
            buf[--sz] = '\0';
        }

        // Check for a binary file
        binary = !binary && !_args.ascii() && memchr(buf, 0, sz) != nullptr;

        ++lineno;
        Match pmatch;
        if (_filter.matchContent(buf, &pmatch)) {
            if (_filter.printContent()) {
                if (!binary) {

                    printf("%s +%d : \"", path.c_str(), lineno);

                    // Print content
                    size_t idx = printMatch(buf, pmatch, nocolor);

                    // Repeat search for more matches
                    while (idx < sz && _filter.matchContent(&buf[idx], &pmatch)) {
                        idx += printMatch(&buf[idx], pmatch, nocolor);
                    }

                    // The remainder of the line
                    if (idx < sz) {
                        char s3[BUF_SIZE];
                        Utils::strncpy_s(s3, BUF_SIZE, &buf[idx], BUF_SIZE - 1);
                        printf("%s\"\n", s3);
                    }
                    else {
                        printf("\"\n");
                    }

                    linesToPrint = _args.extraContent();
                }
                else {
                    // Print only file name and exit
                    printf("%s : binary file matches\n", path.c_str());
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
                printf("%s\n", path.c_str());
                break;
            }
        }

        // Print extra content
        if (linesToPrint > 0) {
            printf("\t%s\n", buf);
            --linesToPrint;
        }
    }
}

bool Search::excludeFileByContent(std::string const & path) const
{
    bool rval = false;
    std::unique_ptr<FILE, decltype(&fclose)> f(Utils::fopen(path.c_str(), "r"), &fclose);
    if (!f) {
        fprintf(stderr, "\033[31mERROR:\033[0m Failed to open file %s : %s\n", path.c_str(), Utils::strerror(errno).c_str());
        return rval;
    }

    char buf[1024];
    while (!rval && fgets(buf, sizeof(buf), f.get()) != nullptr) {
        // Remove trailing CR and LF characters
        size_t sz = strlen(buf);
        while (sz > 0 && (buf[sz - 1] == '\n' || buf[sz - 1] == '\r')) {
            buf[--sz] = '\0';
        }
        rval = _filter.excludeContent(buf);
    }
    return rval;
}

size_t Search::printMatch(char const * buf, Match const & pmatch, bool nocolor) const
{
    size_t const sz = strlen(buf);
    char s1[BUF_SIZE] = "";
    char s2[BUF_SIZE] = "";
    size_t msz = std::min<size_t>(pmatch.position(), BUF_SIZE - 1);
    Utils::strncpy_s(s1, msz + 1, buf, msz);
    size_t idx = msz;
    if (idx < sz) {
        msz = std::min<size_t>(pmatch.length(), BUF_SIZE - 1);
        Utils::strncpy_s(s2, msz + 1, &buf[idx], msz);
        idx += msz;
    }

    printf("%s%s%s%s",
        s1,
        nocolor ? "" : "\033[31m\033[1m",
        s2,
        nocolor ? "" : "\033[0m");

    return idx;
}
