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

    inline ~Config() = default;

    /// True if the configuration is valid and can be used
    inline auto valid() const { return _valid; }

    /// Returns values from the specified section
    auto values(std::string const &section) const -> StringList;

protected:

    using SectionsMap = std::unordered_map<std::string, StringList>;

    bool        _valid;
    SectionsMap _sections;

    auto loadConfig(std::string const &fileName, std::optional<std::string> const &fullPath) -> bool;

    static auto getLocalConfig(std::string const &fileName) -> std::string;
    static auto getSystemConfig(std::string const &fileName) -> std::string;

    /// Trim leading and trailing whitespace
    /// @param[in,out] s String to trim
    static void trim(std::string_view &s);

    /// Check if the string is a section
    /// @param[in,out] s String to check
    /// @return True if the string is a section
    ///
    /// A section is a string that starts with '[' and ends with ']'.
    /// If the string is a section, the string is modified to remove the section markers.
    static auto is_section(std::string_view &s) -> bool;
};

#endif // CONFIG_H
