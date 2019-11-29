#include "args.H"
#include "cmdline.H"
#include "config.H"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace
{
    char const * const usage =
        "USAGE: %1$s [args] [path]\n"
        "\n"
        "where \'path\' is an optional path to start searching. Uses the current working\n"
        "directory if no \'path\' is given. Without any filter arguments uses [path] as\n"
        "the name filter and searches for it in the current directory.\n"
        "\n"
        "args:\n"
        "  -a, --all             print all the matching lines in a file\n"
        "  -A, --ascii           treat all files as ASCII files (no binary file detection)\n"
        "  -c, --content <regex> file content filter (case sensitive)\n"
        "  -C, --icontent <regex> file content filter (case insensitive)\n"
        "  -d, --dir <pattern>   directory name filter (case sensitive)\n"
        "  -D, --idir <pattern>  directory name filter (case insensitive)\n"
        "  -e, --extra <n>       print additional <n> lines after a match with -a\n"
        "  -E, --exec \"<cmd> {}\" execute <cmd> for every matching file\n"
        "                        {} will be replaced with the name of the file\n"
        "  -f, --name <pattern>  file name filter (case sensitive)\n"
        "  -F, --iname <pattern> file name filter (case insensitive)\n"
        "  -h, --help            prints this help message and exits\n"
        "  -n, --not             prefix for the next file name, directory name,\n"
        "                        or file content filter making it an exclude filter\n"
        "  -o, --nocolor         do not highlight search results with colors\n"
        "                        useful when the search results is used as an input\n"
        "                        for some other commands\n"
    #if defined(_AIX)
        "\n"
        "NB! File and directory name filters are always case sensitive on IBM PASE for i\n"
        "due to the fnmatch(3) limitations.\n"
    #endif
        "\n"
        "\n"
        "File and directory name filters use the fnmatch(3) shell wildcard patterns.\n"
        "File content filters use regex(7) regular expressions.\n"
        "\n"
        "Exclude filters exclude directories, file names or content from the subset\n"
        "of files that matches include filters.\n"
        "\n"
        "The --all option allows printing all the matching files in a file with the\n"
        "matching line number and content. Can only be used if the content filter\n"
        "is not empty and exclude content filter is empty.\n"
        "\n"
        "Filters can be prefixed with the --not argument to make them exclude filters.\n"
        "The same can be achieved by prefixing the filter string itself with \'!\'\n"
        "\n"
        "All the filters can be built using predefined lists in a configuration file.\n"
        "These start with \'@\' followed by a name of the list. For example, the following\n"
        "configuration file section defines a list of C++ source files:\n"
        "\n"
        "[@cpp]\n"
        "*.cpp\n"
        "*.h\n"
        "*.C\n"
        "*.H\n"
        "\n"
        "The application tries to use the user\'s configuration file \"~/.config/filefind\". If this is\n"
        "not found, tries to open the global configuration file \"/etc/filefind\".\n"
        "\n"
        "To use a specific configuration file, specify the full path of the file in the\n"
        "FILEFIND_CONFIG environment variable.\n"
        "\n"
        "EXAMPLES:\n"
        "\n"
        "Search for \"*.C\" files in the directory \"~/src/TMTC\" containing the string\n"
        "\"MISCconfig\" excluding directories with names that contain \"unit-tests\"\n"
        "or \"unittest\":\n"
        "\n"
        "> %1$s ~/src/TMTC --name \"*.C\" --content \"MISCconfig\" --not --dir \"unit-tests\" --not --dir \"unittest\"\n"
        "> %1$s ~/src/TMTC -f \"*.C\" -c \"MISCconfig\" -d \'!unit-tests\' -d \'!unittest\'\n"
        "\n"
        "Searching for all the C++ files containing the string \"MISCconfig\" using the predefined list\n"
        "of files \"@cpp\":\n"
        "\n"
        "> %1$s ~/src/TMTC --name \"@cpp\" --content \"MISCconfig\"\n"
        "\n";

    CmdLineOption const opts[] =
    {
        { "all",        CmdLineOption::NoArgument,        'a' },
        { "ascii",      CmdLineOption::NoArgument,        'A' },
        { "content",    CmdLineOption::RequiredArgument,  'c' },
        { "icontent",   CmdLineOption::RequiredArgument,  'C' },
        { "dir",        CmdLineOption::RequiredArgument,  'd' },
        { "idir",       CmdLineOption::RequiredArgument,  'D' },
        { "name",       CmdLineOption::RequiredArgument,  'f' },
        { "iname",      CmdLineOption::RequiredArgument,  'F' },
        { "help",       CmdLineOption::NoArgument,        'h' },
        { "not",        CmdLineOption::NoArgument,        'n' },
        { "extra",      CmdLineOption::RequiredArgument,  'e' },
        { "exec",       CmdLineOption::RequiredArgument,  'E' },
        { "nocolor",    CmdLineOption::NoArgument,        'o' },
        { nullptr,      CmdLineOption::Null,              0 }
    };

    char const * const CONFIG_FILE_NAME_ENV = "FILEFIND_CONFIG";
    char const * const CONFIG_FILE_NAME = "filefind";
}

void Args::printUsage(bool err, char const * appName)
{
    fprintf(err ? stderr : stdout, usage, appName);
}

Args::Args(int argc, char ** argv)
    : _valid(false)
    , _path(".")
    , _allContent(false)
    , _ascii(false)
    , _noColor(false)
    , _extraContent(0)
{
    // Use the configuration file for initial values
    char const * configFileName = getenv(CONFIG_FILE_NAME_ENV);
    Config config(CONFIG_FILE_NAME, configFileName != nullptr ? configFileName : std::string());
    if (config.valid()) {

        // Predefined directory filters
        {
            StringList const values = config.values("dirs");
            StringList::const_iterator it = values.begin();
            for (; it != values.end(); ++it) {
                if (!it->no()) {
                    _inDirs.push_back(*it);
                }
                else {
                    _exDirs.push_back(*it);
                }
            }
        }
        // Predefined file name filters
        {
            StringList const values = config.values("files");
            StringList::const_iterator it = values.begin();
            for (; it != values.end(); ++it) {
                if (!it->no()) {
                    _inFiles.push_back(*it);
                }
                else {
                    _exFiles.push_back(*it);
                }
            }
        }

    }

    char const * appName = argv[0];
    bool no = false;
    CmdLine cmdLine(opts);
    CmdLineArg arg;
    char const * path = nullptr;
    while ((arg = cmdLine.next(argc, argv))) {
        switch (arg.what()) {
            case 'h': {
                printUsage(false, appName);
                return;
            }
            case 'a': {
                _allContent = true;
                break;
            }
            case 'e': {
                char * e = nullptr;
                _extraContent = int(strtol(arg.opt(), &e, 10));
                if (e == nullptr || *e != '\0')
                {
                    fprintf(stderr, "Invalid value \"%s\"\n", arg.opt());
                    return;
                }
                break;
            }
            case 'E': {
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
                String const s(arg.opt(), ic);
                if (s.list()) {
                    // This is a list definition; get content filters from the configuration
                    addFilters(config, s, no, ic, _inContent, _exContent);
                }
                else {
                    no |= s.no();
                    if (!no) {
                        _inContent.push_back(s);
                    }
                    else {
                        _exContent.push_back(s);
                        no = false;
                    }
                }
                break;
            }
            case 'd':
            case 'D': {
                bool const ic = isupper(arg.what());
                String const s(arg.opt(), ic);
                if (s.list()) {
                    // This is a list definition; get directory names from the configuration
                    addFilters(config, s, no, ic, _inDirs, _exDirs);
                }
                else {
                    no |= s.no();
                    if (!no) {
                        _inDirs.push_back(s);
                    }
                    else {
                        _exDirs.push_back(s);
                        no = false;
                    }
                }
                break;
            }
            case 'f':
            case 'F': {
                bool const ic = isupper(arg.what());
                String const s(arg.opt(), ic);
                if (s.list()) {
                    // This is a list definition; get file names from the configuration
                    addFilters(config, s, no, ic, _inFiles, _exFiles);
                }
                else {
                    // This is an individual file name from the command line
                    no |= s.no();
                    if (!no) {
                        _inFiles.push_back(s);
                    }
                    else {
                        _exFiles.push_back(s);
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
                fprintf(stderr, "%s requires an argument\n\n", arg.name());
                printUsage(true, appName);
                return;
            }
            default: {
                fprintf(stderr, "Invalid option %s\n\n", arg.name());
                printUsage(true, appName);
                return;
            }
        }
    }

    // Process the path
    if (path != nullptr) {
        if (_inFiles.empty() && _exFiles.empty()) {
            _inFiles.push_back(path);
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
        fprintf(stderr, "--all option is only allowed if the content filter is not empty.\n");
        return;
    }
    if (_extraContent > 0 && !_allContent) {
        fprintf(stderr, "--extra option is only allowed with the --all option.\n");
        return;
    }
    if (!_exec.empty() && _allContent) {
        fprintf(stderr, "--exec option cannot be used with the --all option.\n");
        return;
    }

    _valid = true;
}

void Args::addFilters(Config const & config,
                      String const & list,
                      bool no,
                      bool ic,
                      std::list<String> & in,
                      std::list<String> & ex)
{
    if (!config.valid()) {
        fprintf(stderr, "No configuration found with list definitions\n");
        return;
    }
    StringList const values = config.values(list);
    if (values.empty()) {
        fprintf(stderr, "List @%s is empty or not found\n", list.c_str());
        return;
    }
    StringList::const_iterator it = values.begin();
    for (; it != values.end(); ++it) {
        no |= it->no();
        if (!no) {
            in.push_back(String(*it, ic));
        }
        else {
            ex.push_back(String(*it, ic));
        }
    }
}
