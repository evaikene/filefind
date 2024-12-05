#ifndef ARGS_H_INCLUDED
#  define ARGS_H_INCLUDED

#include <string>
#include <vector>

class Config;

class String : public std::string {
public:

    inline String(char const *s, bool nc = false)
        : _noCase(nc)
        , _no(false)
        , _list(false)
    {
        setValue(s != nullptr ? std::string(s) : std::string());
    }

    inline String(std::string const &s, bool nc = false)
        : _noCase(nc)
        , _no(false)
        , _list(false)
    {
        setValue(s);
    }

    inline explicit String(String const &o)
        : std::string(o)
        , _noCase(o._noCase)
        , _no(o._no)
        , _list(o._list)
    {}

    inline String &operator=(String const &o)
    {
        if (this != &o) {
            std::string::operator=(o);
            _noCase = o._noCase;
            _no     = o._no;
            _list   = o._list;
        }
        return *this;
    }

    inline bool noCase() const { return _noCase; }

    inline bool no() const { return _no; }

    inline bool list() const { return _list; }

private:

    bool _noCase;
    bool _no;
    bool _list;

    inline void setValue(std::string const &s)
    {
        if (!s.empty() && '@' == s.at(0)) {
            std::string::assign(s);
            _list = true;
        }
        else if (!s.empty() && '!' == s.at(0)) {
            std::string::assign(s, 1, std::string::npos);
            _no = true;
        }
        else {
            std::string::assign(s);
        }
    }
};

using StringList = std::vector<String>;

class Args {
public:

    Args(int argc, char **argv);

    inline ~Args() {}

    /// Flag indicating that the application should exit without performing any searches
    inline bool exit() const { return _exit; }

    /// Flag indicating that command line arguments are valid
    inline bool valid() const { return _valid; }

    inline auto &path() const { return _path; }

    inline auto &includeFiles() const { return _inFiles; }

    inline auto &excludeFiles() const { return _exFiles; }

    inline auto &includeDirs() const { return _inDirs; }

    inline auto &excludeDirs() const { return _exDirs; }

    inline auto &includeContent() const { return _inContent; }

    inline auto &excludeContent() const { return _exContent; }

    inline bool allContent() const { return _allContent; }

    inline int extraContent() const { return _extraContent; }

    inline bool ascii() const { return _ascii; }

    inline bool noColor() const { return _noColor; }

    inline auto &execCmd() const { return _exec; }

private:

    static void printUsage(bool err, char const *appName);

    static void printVersion();

    static void addFilters(Config const &config, String const &list, bool no, bool ic, StringList &in, StringList &ex);

    bool _exit;
    bool _valid;

    std::string _grammar;
    std::string _path;
    StringList  _inFiles;
    StringList  _exFiles;
    StringList  _inDirs;
    StringList  _exDirs;
    StringList  _inContent;
    StringList  _exContent;
    bool        _allContent;
    bool        _ascii;
    bool        _noColor;
    int         _extraContent;
    std::string _exec;
};

#endif // ARGS_H
