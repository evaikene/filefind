#include "args.H"
#include "cmdline.H"
#include "config.H"
#include "utils.H"

#include <fmt/format.h>

#include <stdio.h>
#include <stdlib.h>

#if defined(FF_UNIX)
#  include <unistd.h>
#endif

namespace _ {

static constexpr char const *USAGE = R"(
USAGE: {0} [args] [path]

where 'path' is an optional path to start searching. Uses the current working directory
if no 'path' is given. Without any filter arguments uses [path] as the file name filter
and recursively searches for matching files in the current directory.

args:
  -a, --all             print all the matching lines in a file
  -A, --ascii           treat all files as ASCII files (no binary file detection)
  -c, --content <regex> file content filter (case sensitive)
  -C, --icontent <regex> file content filter (case insensitive)
  -d, --dir <pattern>   directory name filter (case sensitive)
  -D, --idir <pattern>  directory name filter (case insensitive)
  -e, --extra <n>       print additional <n> lines after a match with -a
  -X, --exec "<cmd> {{}}" execute <cmd> for every matching file
                        {{}} will be replaced with the name of the file
  -f, --name <pattern>  file name filter (case sensitive)
  -F, --iname <pattern> file name filter (case insensitive)
  -h, --help            prints this help message and exits
  -n, --not             prefix for the next file name, directory name,
                        or file content filter making it an exclude filter
  -o, --nocolor         do not highlight search results with colors
                        useful when the search results is used as an input
                        for some other commands
  -v, --version         print version number, then exit
{1}
File and directory name filters use the fnmatch(3) shell wildcard patterns on unix-like
operating systems and PathMatchSpecA() on Windows.

File content filters use Perl-style regular expressions.

Exclude filters exclude directories, file names or content from the subset of files
that matches include filters.

The --all option allows printing all the matching lines in a file with the matching
line number and content. Can only be used if the content filter is not empty and
exclude content filter is empty.

Filters can be prefixed with the --not argument to make them exclude filters. The
same can be achieved by prefixing the filter string itself with '!'.

All the filters can be built using predefined lists in a configuration file. These
start with '@' followed by a name of the list. For example, the following configuration
file section defines a list of C++ source files:

[@cpp]
*.cpp
*.h
*.C
*.H

The application tries to use the user's configuration file "~/.config/filefind". If
this is not found, tries to open the global configuration file "/etc/filefind".

To use a specific configuration file, specify the full path of the file in the
FILEFIND_CONFIG environment variable.

EXAMPLES:

Search for "*.C" files in the directory "~/src/TMTC" containing the string "MISCconfig"
excluding directories with names that contain "unit-tests" or "unittest":

> {0} ~/src/TMTC --name "*.C" --content "MISCconfig" --not --dir "unit-tests" --not --dir "unittest"
> {0} ~/src/TMTC -f "*.C" -c "MISCconfig" -d '!unit-tests' -d '!unittest'

Searching for all the C++ files containing the string "MISCconfig" using the predefined list
of files "@cpp":

> {0} ~/src/TMTC --name "@cpp" --content "MISCconfig"
)";

#if defined(FF_AIX)
static constexpr char const *NOTES                                                = R"(
NB! File and directory name filters are always case sensitive on IBM PASE for i
due to the fnmatch(3) limitations.
)";
elif                         defined(FF_WIN32) static constexpr char const *NOTES = R"(
NB! File and directory name filters are always case insensitive on Windows.
)";
#else
static constexpr char const *NOTES = "";
#endif

static constexpr CmdLineOption const opts[] = {
    {"all",      CmdLineOption::NoArgument,       'a'},
    {"ascii",    CmdLineOption::NoArgument,       'A'},
    {"content",  CmdLineOption::RequiredArgument, 'c'},
    {"icontent", CmdLineOption::RequiredArgument, 'C'},
    {"dir",      CmdLineOption::RequiredArgument, 'd'},
    {"idir",     CmdLineOption::RequiredArgument, 'D'},
    {"extra",    CmdLineOption::RequiredArgument, 'e'},
    {"exec",     CmdLineOption::RequiredArgument, 'X'},
    {"name",     CmdLineOption::RequiredArgument, 'f'},
    {"iname",    CmdLineOption::RequiredArgument, 'F'},
    {"help",     CmdLineOption::NoArgument,       'h'},
    {"not",      CmdLineOption::NoArgument,       'n'},
    {"nocolor",  CmdLineOption::NoArgument,       'o'},
    {"version",  CmdLineOption::NoArgument,       'v'},
    {nullptr,    CmdLineOption::Null,             0  }
};

static constexpr char const *CONFIG_FILE_NAME_ENV = "FILEFIND_CONFIG";
static constexpr char const *CONFIG_FILE_NAME     = "filefind";

} // namespace _

void Args::printUsage(bool err, char const *appName)
{
    fmt::print(err ? stderr : stdout, _::USAGE, appName, _::NOTES);
}

void Args::printVersion()
{
    constexpr char const *rx_version = "re2";
    fmt::println("{} ({})", PACKAGE_STRING, rx_version);
}

Args::Args(int argc, char **argv)
    : _valid(true)
    , _exit(true)
    , _path(".")
    , _allContent(false)
    , _ascii(false)
#if defined(FF_WIN32)
    , _noColor(true)
#else
    , _noColor(false)
#endif
    , _extraContent(0)
{
    // Use the configuration file for initial values
    auto const configFileName = Utils::getenv(_::CONFIG_FILE_NAME_ENV);
    Config     config(_::CONFIG_FILE_NAME, configFileName);
    if (config.valid()) {

        // Predefined directory filters
        {
            auto const           values = config.values("dirs");
            auto it     = values.begin();
            for (; it != values.end(); ++it) {
                ArgVal v{*it};
                if (!v.no()) {
                    _inDirs.push_back(std::move(v));
                }
                else {
                    _exDirs.push_back(std::move(v));
                }
            }
        }
        // Predefined file name filters
        {
            auto const           values = config.values("files");
            auto it     = values.begin();
            for (; it != values.end(); ++it) {
                ArgVal v{*it};
                if (v.no()) {
                    _inFiles.push_back(std::move(v));
                }
                else {
                    _exFiles.push_back(std::move(v));
                }
            }
        }
    }

    char const *appName = argv[0];
    bool        no      = false;
    CmdLine     cmdLine(*_::opts);
    CmdLineArg  arg;
    char const *path = nullptr;
    while ((arg = cmdLine.next(argc, argv))) {
        switch (arg.what()) {
            case 'h': {
                printUsage(false, appName);
                return;
            }
            case 'v': {
                printVersion();
                return;
            }
            case 'a': {
                _allContent = true;
                break;
            }
            case 'e': {
                char *e       = nullptr;
                _extraContent = int(strtol(arg.opt(), &e, 10));
                if (e == nullptr || *e != '\0') {
                    fmt::println(stderr, "Invalid value \"{}\"", arg.opt());
                    _valid = false;
                    return;
                }
                break;
            }
            case 'X': {
                _exec = arg.opt();
                break;
            }
            case 'A': {
                _ascii = true;
                break;
            }
            case 'n': {
                no = true;
                break;
            }
            case 'c':
            case 'C': {
                bool const ic = isupper(arg.what());
                ArgVal     s{arg.opt(), ic};
                if (s.list()) {
                    // This is a list definition; get content filters from the configuration
                    addFilters(config, s, no, ic, _inContent, _exContent);
                }
                else {
                    no |= s.no();
                    if (!no) {
                        _inContent.push_back(std::move(s));
                    }
                    else {
                        _exContent.push_back(std::move(s));
                        no = false;
                    }
                }
                break;
            }
            case 'd':
            case 'D': {
                bool const ic = isupper(arg.what());
                ArgVal     s{arg.opt(), ic};
                if (s.list()) {
                    // This is a list definition; get directory names from the configuration
                    addFilters(config, s, no, ic, _inDirs, _exDirs);
                }
                else {
                    no |= s.no();
                    if (!no) {
                        _inDirs.push_back(std::move(s));
                    }
                    else {
                        _exDirs.push_back(std::move(s));
                        no = false;
                    }
                }
                break;
            }
            case 'f':
            case 'F': {
                bool const ic = isupper(arg.what());
                ArgVal     s(arg.opt(), ic);
                if (s.list()) {
                    // This is a list definition; get file names from the configuration
                    addFilters(config, s, no, ic, _inFiles, _exFiles);
                }
                else {
                    // This is an individual file name from the command line
                    no |= s.no();
                    if (!no) {
                        _inFiles.push_back(std::move(s));
                    }
                    else {
                        _exFiles.push_back(std::move(s));
                        no = false;
                    }
                }
                break;
            }
            case 'o': {
                _noColor = true;
                break;
            }
            case CmdLineArg::NO_OPTION: {
                path = arg.name();
                break;
            }
            case CmdLineArg::REQUIRES_ARGUMENT: {
                fmt::println(stderr, "\"{}\" requires an argument\n", arg.name());
                printUsage(true, appName);
                _valid = false;
                return;
            }
            default: {
                fmt::println(stderr, "Invalid option \"{}\"\n", arg.name());
                printUsage(true, appName);
                _valid = false;
                return;
            }
        }
    }

    // Process the path
    if (path != nullptr) {
        if (_inFiles.empty() && _exFiles.empty()) {
            _inFiles.emplace_back(path);
        }
        else {
            _path = path;
            // Remove trailing slashes
            while (_path.size() > 1 && _path.at(_path.size() - 1) == '/') {
                _path.resize(_path.size() - 1);
            }
        }
    }

    if (_allContent && _inContent.empty()) {
        fmt::println(stderr, "--all option is only allowed if the content filter is not empty.");
        _valid = false;
        return;
    }
    if (_extraContent > 0 && !_allContent) {
        fmt::println(stderr, "--extra option is only allowed with the --all option.");
        _valid = false;
        return;
    }
    if (_exec && _allContent) {
        fmt::println(stderr, "--exec option cannot be used with the --all option.");
        _valid = false;
        return;
    }

    _exit = false;
}

void Args::addFilters(Config const &config, ArgVal const &list, bool no, bool ic, ArgValList &in, ArgValList &ex)
{
    if (!config.valid()) {
        fmt::println(stderr, "No configuration found with list definitions");
        return;
    }
    auto const values = config.values(list.str());
    if (values.empty()) {
        fmt::println(stderr, "List @{} is empty or not found", list);
        return;
    }
    auto it = values.begin();
    for (; it != values.end(); ++it) {
        ArgVal v{*it, ic};
        no |= v.no();
        if (!no) {
            in.push_back(std::move(v));
        }
        else {
            ex.push_back(std::move(v));
        }
    }
}
