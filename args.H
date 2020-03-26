#ifndef ARGS_H
#define ARGS_H

#include <string>
#include <list>

class Config;

class String : public std::string
{
public:

    inline String(char const * s, bool nc = false)
        : _noCase(nc)
        , _no(false)
        , _list(false)
    {
        setValue(s != nullptr ? std::string(s) : std::string());
    }
    inline String(std::string const & s, bool nc = false)
        : _noCase(nc)
        , _no(false)
        , _list(false)
    {
        setValue(s);
    }
    inline explicit String(String const & o)
        : std::string(o)
        , _noCase(o._noCase)
        , _no(o._no)
        , _list(o._list)
    {}

    inline String & operator= (String const & o)
    {
        if (this != &o)
        {
            std::string::operator= (o);
            _noCase = o._noCase;
            _no = o._no;
            _list = o._list;
        }
        return *this;
    }

    inline bool noCase() const
    {
        return _noCase;
    }
    inline bool no() const
    {
        return _no;
    }
    inline bool list() const
    {
        return _list;
    }

private:

    bool _noCase;
    bool _no;
    bool _list;

    inline void setValue(std::string const & s)
    {
        if (!s.empty() && '@' == s.at(0)) {
            std::string::assign(s);
            _list = true;
        }
        else if (!s.empty() && '!' == s.at(0))
        {
            std::string::assign(s, 1, std::string::npos);
            _no = true;
        }
        else
        {
            std::string::assign(s);
        }
    }
};

typedef std::list<String> StringList;

class Args
{
public:

    Args(int argc, char ** argv);
    inline ~Args()
    {}

    inline bool valid() const
    {
        return _valid;
    }

    inline std::string const & grammar() const
    {
        return _grammar;
    }
    inline std::string const & path() const
    {
        return _path;
    }
    inline std::list<String> const & includeFiles() const
    {
        return _inFiles;
    }
    inline std::list<String> const & excludeFiles() const
    {
        return _exFiles;
    }
    inline std::list<String> const & includeDirs() const
    {
        return _inDirs;
    }
    inline std::list<String> const & excludeDirs() const
    {
        return _exDirs;
    }
    inline std::list<String> const & includeContent() const
    {
        return _inContent;
    }
    inline std::list<String> const & excludeContent() const
    {
        return _exContent;
    }
    inline bool allContent() const
    {
        return _allContent;
    }
    inline int extraContent() const
    {
        return _extraContent;
    }
    inline bool ascii() const
    {
        return _ascii;
    }
    inline bool noColor() const
    {
        return _noColor;
    }
    inline std::string const & execCmd() const
    {
        return _exec;
    }

private:

    static void printUsage(bool err, char const * appName);

    static void printVersion();

    static void addFilters(Config const & config,
                           String const & list,
                           bool no,
                           bool ic,
                           std::list<String> & in,
                           std::list<String> & ex);

    bool _valid;

    std::string _grammar;
    std::string _path;
    std::list<String> _inFiles;
    std::list<String> _exFiles;
    std::list<String> _inDirs;
    std::list<String> _exDirs;
    std::list<String> _inContent;
    std::list<String> _exContent;
    bool _allContent;
    bool _ascii;
    bool _noColor;
    int _extraContent;
    std::string _exec;
};

#endif // ARGS_H
