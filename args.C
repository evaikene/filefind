#include "args.H"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

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
        "                        for some other commands"
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
        "EXAMPLES:\n"
        "\n"
        "Search for \"*.C\" files in the directory \"~/src/TMTC\" containing the string\n"
        "\"MISCconfig\" excluding directories with names that contain \"unit-tests\"\n"
        "or \"unittest\"\n"
        "\n"
        "> %1$s ~/src/TMTC --name \"*.C\" --content \"MISCconfig\" --not --dir \"unit-tests\" --not --dir \"unittest\"\n"
        "> %1$s ~/src/TMTC -f \"*.C\" -c \"MISCconfig\" -d \'!unit-tests\' -d \'!unittest\'\n"
        "\n";

    char const * const shortOpts = ":aAc:C:d:D:e:E:f:F:hno";
    struct option const longOpts[] =
    {
        { "all",        no_argument,        nullptr, 'a' },
        { "ascii",      no_argument,        nullptr, 'A' },
        { "content",    required_argument,  nullptr, 'c' },
        { "icontent",   required_argument,  nullptr, 'C' },
        { "dir",        required_argument,  nullptr, 'd' },
        { "idir",       required_argument,  nullptr, 'D' },
        { "name",       required_argument,  nullptr, 'f' },
        { "iname",      required_argument,  nullptr, 'F' },
        { "help",       no_argument,        nullptr, 'h' },
        { "not",        no_argument,        nullptr, 'n' },
        { "extra",      required_argument,  nullptr, 'e' },
        { "exec",       required_argument,  nullptr, 'E' },
        { "nocolor",    no_argument,        nullptr, 'o' },
        { nullptr,      0,                  nullptr, 0 }
    };

}

void Args::printUsage(bool err, char const * appName)
{
    fprintf(err ? stderr : stdout, usage, appName);
}

Args::Args(int argc, char ** argv)
    : m_valid(false)
    , m_path(".")
    , m_allContent(false)
    , m_ascii(false)
    , m_noColor(false)
    , m_extraContent(0)
{
    char const * appName = argv[0];
    int c = 0;
    int idx = 0;
    bool no = false;
    while ((c = getopt_long(argc, argv, shortOpts, longOpts, &idx)) != -1)
    {
        switch (c)
        {
            case 'h':
            {
                printUsage(false, appName);
                return;
            }
            case 'a':
            {
                m_allContent = true;
                break;
            }
            case 'e':
            {
                char * e = nullptr;
                m_extraContent = int(strtol(optarg, &e, 10));
                if (e == nullptr || *e != '\0')
                {
                    fprintf(stderr, "Invalid value \"%s\"\n", optarg);
                    return;
                }
                break;
            }
            case 'E':
            {
                m_exec = optarg;
                break;
            }
            case 'A':
            {
                m_ascii = true;
                break;
            }
            case 'n':
            {
                no = true;
                break;
            }
            case 'c':
            case 'C':
            {
                String const s(optarg, isupper(c));
                no |= s.no();
                if (!no)
                {
                    m_inContent.push_back(s);
                }
                else
                {
                    m_exContent.push_back(s);
                    no = false;
                }
                break;
            }
            case 'd':
            case 'D':
            {
                String const s(optarg, isupper(c));
                no |= s.no();
                if (!no)
                {
                    m_inDirs.push_back(s);
                }
                else
                {
                    m_exDirs.push_back(s);
                    no = false;
                }
                break;
            }
            case 'f':
            case 'F':
            {
                String const s(optarg, isupper(c));
                no |= s.no();
                if (!no)
                {
                    m_inFiles.push_back(s);
                }
                else
                {
                    m_exFiles.push_back(s);
                    no = false;
                }
                break;
            }
            case 'o':
            {
                m_noColor = true;
                break;
            }
            case ':':
            {
                fprintf(stderr, "Missing argument\n\n");
                printUsage(true, appName);
                return;
            }
            default:
            {
                fprintf(stderr, "Invalid option\n\n");
                printUsage(true, appName);
                return;
            }
        }
    }

    if (optind < argc)
    {
        if (m_inFiles.empty() && m_exFiles.empty())
        {
            m_inFiles.push_back(argv[optind]);
        }
        else
        {
            m_path = argv[optind];
            // Remove trailing slashes
            while (m_path.size() > 1 && m_path.at(m_path.size() - 1) == '/')
            {
                m_path.resize(m_path.size() - 1);
            }
        }
    }

    if (m_allContent && m_inContent.empty())
    {
        fprintf(stderr, "--all option is only allowed if the content filter is not empty.\n");
        return;
    }
    if (m_extraContent > 0 && !m_allContent)
    {
        fprintf(stderr, "--extra option is only allowed with the --all option.\n");
        return;
    }
    if (!m_exec.empty() && m_allContent)
    {
        fprintf(stderr, "--exec option cannot be used with the --all option.\n");
        return;
    }

    m_valid = true;
}
