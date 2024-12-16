#include "cmdline.H"

#include <cstring>

CmdLine::CmdLine(CmdLineOption const &opts)
    : _idx(0)
    , _opts(opts)
{}

auto CmdLine::next(int argc, char **argv) -> CmdLineArg
{
    ++_idx;
    if (_idx >= argc) {
        return {};
    }

    char const * arg = argv[_idx];

    // Is it a command line argument?
    if (*arg != '-') {
        return {CmdLineArg::NO_OPTION, argv[_idx]};
    }

    // Otherwise it is a short or long command line option
    bool longOpt = false;
    if (*arg == '-') {
        ++arg;
        if (*arg == '\0') {
            return {CmdLineArg::INVALID_OPTION, argv[_idx]};
        }
    }
    if (*arg == '-') {
        ++arg;
        if (*arg == '\0') {
            return {CmdLineArg::INVALID_OPTION, argv[_idx]};
        }
        longOpt = true;
    }

    // The rest shall be the command line option
    auto const *opt = &_opts;
    char const * end = nullptr;
    for (; opt->shortName != '\0'; ++opt) {
        if (longOpt) {
            if (opt->longName != nullptr && strncmp(arg, opt->longName, strlen(opt->longName)) == 0) {
                end = arg + strlen(opt->longName);
                // Shall end with '\0' or '='
                if (*end == '\0' || *end == '=') {
                    break;
                }
                // Otherwise keep looking for matches
                end = nullptr;
            }
        }
        else {
            if (*arg == opt->shortName) {
                end = arg + 1;
                break;
            }
        }
    }

    // Is it an invalid option
    if (opt->shortName == '\0' || end == nullptr) {
        return {CmdLineArg::INVALID_OPTION, argv[_idx]};
    }

    // Does it require arguments?
    if (opt->flag == CmdLineOption::RequiredArgument) {
        // Shall end with '=' or '\0'
        if (*end == '=') {
            ++end;
            return {opt->shortName, opt->longName, end};
        }
        if (*end == '\0') {
            // Argument is next
            ++_idx;
            if (_idx >= argc) {
                return {CmdLineArg::REQUIRES_ARGUMENT, argv[_idx - 1]};
            }
            return {opt->shortName, opt->longName, argv[_idx]};
        }
    }

    // Does not require arguments; end shall be '\0'
    if (*end == '\0') {
        return {opt->shortName, opt->longName};
    }

    // Otherwise this is an invalid option
    return {CmdLineArg::INVALID_OPTION, argv[_idx]};
}
