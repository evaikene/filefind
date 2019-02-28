#ifndef ARGS_H
#define ARGS_H

#include <string>
#include <list>

class String : public std::string
{
public:

    inline String(char const * s, bool nc = false)
        : m_noCase(nc)
        , m_no(false)
    {
        setValue(s != nullptr ? std::string(s) : std::string());
    }
    inline String(std::string const & s, bool nc = false)
        : m_noCase(nc)
        , m_no(false)
    {
        setValue(s);
    }
    inline explicit String(String const & o)
        : std::string(o)
        , m_noCase(o.m_noCase)
        , m_no(o.m_no)
    {}

    inline String & operator= (String const & o)
    {
        if (this != &o)
        {
            std::string::operator= (o);
            m_noCase = o.m_noCase;
            m_no = o.m_no;
        }
        return *this;
    }

    inline bool noCase() const
    {
        return m_noCase;
    }
    inline bool no() const
    {
        return m_no;
    }

private:

    bool m_noCase;
    bool m_no;

    inline void setValue(std::string const & s)
    {
        if (!s.empty() && '!' == s.at(0))
        {
            std::string::assign(s, 1, std::string::npos);
            m_no = true;
        }
        else
        {
            std::string::assign(s);
        }
    }
};


class Args
{
public:

    Args(int argc, char ** argv);
    inline ~Args()
    {}

    inline bool valid() const
    {
        return m_valid;
    }

    inline std::string const & path() const
    {
        return m_path;
    }
    inline std::list<String> const & includeFiles() const
    {
        return m_inFiles;
    }
    inline std::list<String> const & excludeFiles() const
    {
        return m_exFiles;
    }
    inline std::list<String> const & includeDirs() const
    {
        return m_inDirs;
    }
    inline std::list<String> const & excludeDirs() const
    {
        return m_exDirs;
    }
    inline std::list<String> const & includeContent() const
    {
        return m_inContent;
    }
    inline std::list<String> const & excludeContent() const
    {
        return m_exContent;
    }
    inline bool allContent() const
    {
        return m_allContent;
    }
    inline int extraContent() const
    {
        return m_extraContent;
    }
    inline bool ascii() const
    {
        return m_ascii;
    }
    inline bool noColor() const
    {
        return m_noColor;
    }
    inline std::string const & execCmd() const
    {
        return m_exec;
    }

private:

    static void printUsage(bool err, char const * appName);

    bool m_valid;

    std::string m_path;
    std::list<String> m_inFiles;
    std::list<String> m_exFiles;
    std::list<String> m_inDirs;
    std::list<String> m_exDirs;
    std::list<String> m_inContent;
    std::list<String> m_exContent;
    bool m_allContent;
    bool m_ascii;
    bool m_noColor;
    int m_extraContent;
    std::string m_exec;
};

#endif // ARGS_H

