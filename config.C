#include "config.H"
#include "utils.H"

#include <fmt/format.h>

#include <stdio.h>

namespace _ {

constexpr size_t BUF_SIZE = 4096;

}

Config::Config(std::string const &fileName, std::optional<std::string> const &fullPath)
    : _valid(false)
{
#if defined(FF_UNIX)
    _valid = loadConfig(fileName, fullPath);
#endif
}

auto Config::values(std::string const & section) const -> StringList
{
    SectionsMap::const_iterator it = _sections.find(section);
    if (it == _sections.end()) {
        return {};
    }
    return it->second;
}

bool Config::loadConfig(std::string const &fileName, std::optional<std::string> const &fullPath)
{
    auto f = Utils::InvalidFilePtr();
    if (fullPath) {
		f = Utils::fopen(*fullPath, "r");
	} else {
		f = Utils::fopen(getLocalConfig(fileName), "r");
		if (!f) {
			f = Utils::fopen(getSystemConfig(fileName), "r");
        }
	}
    if (!f) {
        return false;
    }

    // Add the default section without a name
    auto [el, ok] = _sections.emplace(std::string{}, StringList{});
    auto *section = &el->second;

    char buf[_::BUF_SIZE];
    while (fgets(buf, _::BUF_SIZE, f.get()) != nullptr) {

        std::string_view line(buf);

        // Trim leading and trailing whitespace
        trim(line);

        // Ignore empty lines
        if (line.empty()) {
            continue;
        }

        // Ignore comments that start with '#' or ';'
        if (line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Is it a section?
        if (is_section(line)) {
            auto const name = std::string{line.data(), line.size()};
            auto it = _sections.find(name);
            if (it == _sections.end()) {
                std::tie(it, ok) = _sections.emplace(name, StringList{});
            }
            section = &it->second;
            continue;
        }

        // Must be a value belonging to the currently active section
        section->emplace_back(line.data(), line.size());
    }

    return true;
}

std::string Config::getLocalConfig(std::string const &filename)
{
    // Determine the home directory
    auto const home = Utils::getenv("HOME");
    return fmt::format("{}/.config/{}", home ? *home : "", filename);
}

std::string Config::getSystemConfig(std::string const & filename)
{
    return fmt::format("/etc/{}", filename);
}

void Config::trim(std::string_view &s)
{
    static constexpr char const *chars = "\t\n\v\f\r ";
    auto pos = s.find_first_not_of(chars);
    if (pos != std::string_view::npos) {
        s.remove_prefix(pos);
    }
    else {
        s.remove_prefix(s.size());
        return;
    }
    pos = s.find_last_not_of(chars);
    if (pos != std::string_view::npos) {
        s.remove_suffix(s.size() - pos - 1);
    }
}

bool Config::is_section(std::string_view &s)
{
    if (s.empty() || s[0] != '[' || s[s.size() - 1] != ']') {
        return false;
    }
    s.remove_prefix(1);
    s.remove_suffix(1);
    trim(s);
    return true;
}
