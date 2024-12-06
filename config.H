#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief The configuration class
 *
 * Loads configuration values from an external configuration file.
 * If a full path is given, uses the specific configuration file.
 * If only a file name is given, searches for the specified file in the following locations:
 *   ~/.config/
 *   /etc/
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
class Config {
public:

    using StringList = std::vector<std::string>;

    Config(std::string const &fileName, std::optional<std::string> const &fullPath = std::nullopt);

    inline ~Config() {}

    /// True if the configuration is valid and can be used
    inline bool valid() const { return _valid; }

    /// Returns values from the specified section
    auto values(std::string const &section) const -> StringList;

protected:

    using SectionsMap = std::unordered_map<std::string, StringList>;

    bool        _valid;
    SectionsMap _sections;

    bool loadConfig(std::string const &fileName, std::optional<std::string> const &fullPath);

    static auto getLocalConfig(std::string const &fileName) -> std::string;
    static auto getSystemConfig(std::string const &fileName) -> std::string;

    static void trim(std::string_view &s);
    static bool is_section(std::string_view &s);
};

#endif // CONFIG_H
