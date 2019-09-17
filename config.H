#ifndef CONFIG_H
#define CONFIG_H

#include "args.H"

#include <list>
#include <map>

/**
 * @brief The configuration class
 *
 * Loads configuration values from external configuration files (in the following order):
 *   ~/.config/<fileName>
 *   /etc/<fileName>
 *
 * The configuration file has sections followed by a number of values.
 * For example:
 * [section1]
 * value1
 * value2
 * [section2]
 * value3
 *
 * Empty lines and lines starting with '#' or ';' are ignored.
 * Leading and trailing whitespace in section names and values is ignored.
 * Values can start with '!' making it a 'not' value.
 */
class Config
{
public:

    Config(std::string const & fileName);
    inline ~Config()
    {}

    /// True if the configuration is valid and can be used
    inline bool valid() const
    {
        return _valid;
    }

    /// Returns values from the specified section
    StringList values(std::string const & section) const;


protected:

    typedef std::map<std::string, StringList> SectionsMap;

    bool _valid;
    SectionsMap _sections;

    bool loadConfig(std::string const & fileName);

    static std::string getLocalConfig(std::string const & fileName);
    static std::string getSystemConfig(std::string const & fileName);

    static void trim(std::string & s);

    static bool isSection(std::string & s);

};

#endif // CONFIG_H
