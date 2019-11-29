#ifndef CMDLINE_H
#define CMDLINE_H

class CmdLineArg
{
public:

    static char const EMPTY = '\0';
    static char const NO_OPTION = '\1';
    static char const INVALID_OPTION = '\2';
    static char const REQUIRES_ARGUMENT = '\3';

    inline CmdLineArg()
        : _what(EMPTY)
        , _name(nullptr)
        , _opt(nullptr)
    {}

    inline CmdLineArg(char what, char const * name, char const * opt = nullptr)
        : _what(what)
        , _name(name)
        , _opt(opt)
    {}

    inline operator bool () const
    {
        return (_what != EMPTY);
    }

    inline char what() const
    {
        return _what;
    }

    inline char const * name() const
    {
        return _name;
    }

    inline char const * opt() const
    {
        return _opt;
    }

private:

    char _what;
    char const * _name;
    char const * _opt;
};

struct CmdLineOption
{
    enum Flag {
        Null,
        NoArgument,
        RequiredArgument
    };

    char const * longName;
    Flag flag;
    char const shortName;
};

class CmdLine
{
public:

    CmdLine(CmdLineOption const * opts);

    CmdLineArg const next(int argc, char * argv[]);

private:

    int _idx;

    CmdLineOption const * _opts;

};

#endif
