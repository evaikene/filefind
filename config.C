#include "config.H"

#include <sstream>
#include <fstream>

Config::Config(std::string const & fileName)
    : _valid(false)
{
    _valid = loadConfig(fileName);
}

StringList Config::values(std::string const & section) const
{
    SectionsMap::const_iterator it = _sections.find(section);
    if (it == _sections.end()) {
        return StringList();
    }
    return it->second;
}

bool Config::loadConfig(std::string const & fileName)
{
    std::ifstream f;
    f.open(getLocalConfig(fileName).c_str());
    if (!f.is_open()) {
        f.open(getSystemConfig(fileName).c_str());
        if (!f.is_open()) {
            return false;
        }
    }

    // Add the default section without a name
    _sections[std::string()] = StringList();
    StringList * section = &_sections[std::string()];

    std::string line;
    while (std::getline(f, line)) {

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
        if (isSection(line)) {
            SectionsMap::const_iterator it = _sections.find(line);
            if (it == _sections.end()) {
                _sections[line] = StringList();
            }
            section = &_sections[line];
            continue;
        }

        // Must be a value belonging to the currently active section
        section->push_back(line);
    }

    return true;
}

std::string Config::getLocalConfig(std::string const & fileName)
{
    // Determine the home directory
    char const * homeDir = getenv("HOME");
    return std::string(homeDir) + "/.config/" + fileName;
}

std::string Config::getSystemConfig(std::string const & fileName)
{
    return std::string("/etc/") + fileName;
}

void Config::trim(std::string & s)
{
    char const * const chars = "\t\n\v\f\r ";
    s.erase(0, s.find_first_not_of(chars));
    s.erase(s.find_last_not_of(chars) + 1);
}

bool Config::isSection(std::string & s)
{
    if (s.empty() || s[0] != '[' || s[s.size() - 1] != ']') {
        return false;
    }
    s.erase(0, 1);
    s.erase(s.size() - 1, 1);
    trim(s);
    return true;
}
