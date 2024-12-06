#ifndef ARGS_H_INCLUDED
#  define ARGS_H_INCLUDED

#include <fmt/format.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

class Config;

/// Command line argument value
class ArgVal {
public:

    /// Ctor
    /// @param[in] s Argument value
    /// @param[in] nc Case insensitive
    inline ArgVal(char const *s, bool nc = false)
        : _flags(nc ? Flags::NO_CASE : 0)
    {
        set_value(s);
    }

    /// Ctor
    /// @param[in] s Argument value
    /// @param[in] nc Case insensitive
    inline ArgVal(std::string const &s, bool nc = false)
        : _flags(nc ? Flags::NO_CASE : 0)
    {
        set_value(s);
    }

    /// Move ctor
    inline ArgVal(ArgVal &&rhs)
    {
        swap(rhs);
    }

    /// Move assignment
    inline ArgVal &operator=(ArgVal &&rhs)
    {
        if (this != &rhs) {
            clear();
            swap(rhs);
        }
        return *this;
    }

    /// Avoid copying
    ArgVal(ArgVal const &) = delete;
    ArgVal &operator=(ArgVal const &) = delete;

    /// Returns the string value
    inline auto &str() const { return _value; }

    inline bool noCase() const { return _flags & Flags::NO_CASE; };

    inline bool no() const { return _flags & Flags::NOT; };

    inline bool list() const { return _flags & Flags::LIST; };

private:

    /// Flags
    enum Flags : uint8_t {
        NO_CASE = 0x01, ///< Case insensitive
        NOT     = 0x02, ///< Exclude
        LIST    = 0x04  ///< List
    };

    uint8_t _flags = 0;

    std::string _value;

    inline void clear()
    {
        _flags = 0;
        _value.clear();
    }

    inline void swap(ArgVal &rhs)
    {
        std::swap(_flags, rhs._flags);
        _value.swap(rhs._value);
    }

    inline void set_value(char const *s)
    {
        if (s == nullptr || s[0] == '\0') {
            return;
        }
        if (s[0] == '@') {
            set_value(std::string{s});
            _flags |= Flags::LIST;
        }
        else if (s[0] == '!') {
            set_value(std::string{s + 1});
            _flags |= Flags::NOT;
        }
        else {
            set_value(std::string{s});
        }
    }

    inline void set_value(std::string const &s)
    {
        _value = s;
    }

};

template <>
struct fmt::formatter<ArgVal> : public fmt::formatter<std::string_view> {
    template <typename FormatContext>
    constexpr auto parse(FormatContext &ctx)
    {
        return fmt::formatter<std::string_view>::parse(ctx);
    }

    template <typename FormatContext>
    auto format(ArgVal const &val, FormatContext &ctx) const
    {
        return fmt::formatter<std::string_view>::format(val.str(), ctx);
    }
};

using ArgValList = std::vector<ArgVal>;

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

    static void addFilters(Config const &config, ArgVal const &list, bool no, bool ic, ArgValList &in, ArgValList &ex);

    bool _exit;
    bool _valid;

    std::string                _path;
    ArgValList                 _inFiles;
    ArgValList                 _exFiles;
    ArgValList                 _inDirs;
    ArgValList                 _exDirs;
    ArgValList                 _inContent;
    ArgValList                 _exContent;
    bool                       _allContent;
    bool                       _ascii;
    bool                       _noColor;
    int                        _extraContent;
    std::optional<std::string> _exec;
};

#endif // ARGS_H
