#ifndef CMDLINE_H
#define CMDLINE_H

class CmdLineArg {
public:

    static constexpr char EMPTY             = '\0';
    static constexpr char NO_OPTION         = '\1';
    static constexpr char INVALID_OPTION    = '\2';
    static constexpr char REQUIRES_ARGUMENT = '\3';

    inline CmdLineArg() = default;

    inline CmdLineArg(char what, char const *name, char const *opt = nullptr)
        : _what(what)
        , _name(name)
        , _opt(opt)
    {}

    inline operator bool() const { return (_what != EMPTY); }

    inline auto what() const { return _what; }

    inline auto name() const { return _name; }

    inline auto opt() const { return _opt; }

private:

    char        _what = EMPTY;
    char const *_name = nullptr;
    char const *_opt  = nullptr;
};

struct CmdLineOption {

    enum Flag {
        Null,
        NoArgument,
        RequiredArgument
    };

    char const *longName  = nullptr;
    Flag        flag      = Flag::Null;
    char const  shortName = '\0';
};

class CmdLine {
public:

    CmdLine(CmdLineOption const &opts);

    auto next(int argc, char *argv[]) -> CmdLineArg const;

private:

    int _idx;

    CmdLineOption const &_opts;
};

#endif
