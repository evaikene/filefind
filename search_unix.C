#include "args.H"
#include "filter.H"
#include "search_unix.H"
#include "utils.H"

#include <fmt/color.h>
#include <fmt/format.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#if defined(__APPLE__)
#  include <sys/syslimits.h>
#endif

#if defined(FF_AIX)
// struct dirent on AIX does not have the d_type field nor defines for its values
#  include <sys/vnode.h>
#  define DT_UNKNOWN 0
#  define DT_FIFO    1
#  define DT_DIR     2
#  define DT_REG     3
#  define DT_LNK     10
#  define DT_SOCK    12
#endif

namespace _ {

void closedir(DIR *d)
{
    if (nullptr != d) {
        ::closedir(d);
    }
}

} // namespace _

SearchUnix::SearchUnix(Args const &args)
    : Search(args)
{}

SearchUnix::~SearchUnix()
{}

/// Executes the command for a file
void SearchUnix::exec_cmd(std::string const &cmd, std::string const &path) const
{
    std::string cmdline(cmd);
    size_t      pos = 0;
    while ((pos = cmdline.find("{}", pos)) != std::string::npos) {
        cmdline.replace(pos, 2, path);
        pos = pos - 2 + path.size();
    }
    if (system(cmdline.c_str()) != 0) {}
}

void SearchUnix::find_files(std::string const &root, std::string const &path, bool dirMatch) const
{
    std::string fullPath(root);
    if (!fullPath.empty()) {
        fullPath.append("/");
    }
    if (!path.empty()) {
        fullPath.append(path);
        if (path.at(path.size() - 1) != '/') {
            fullPath.append(1, '/');
        }
    }
    std::unique_ptr<DIR, decltype(&_::closedir)> dir(opendir(fullPath.c_str()), &_::closedir);
    if (!dir) {
        fmt::println(stderr,
                     "{} Failed to open file {} : {}",
                     fmt::styled("ERROR:", fmt::fg(fmt::color::red)),
                     fullPath,
                     Utils::strerror(errno));
        return;
    }

    auto const &cmd = _args.execCmd();

    auto const           nocolor = _args.noColor();
    struct dirent const *dent    = nullptr;
    while ((dent = readdir(dir.get())) != nullptr) {
        char const *d_name = dent->d_name;
#if defined(FF_AIX)
        auto const d_type = getType(fullPath + d_name, DT_UNKNOWN);
#else
        auto const d_type = getType(fullPath + d_name, dent->d_type);
#endif

        if (DT_LNK == d_type) {
            if (!_filter.match_file(d_name)) {
                // Skip symbolic links that do not match the file name filter
                continue;
            }
            std::string const filePath(fullPath + d_name);
            char              buf[PATH_MAX];
            ssize_t           sz = PATH_MAX - 1;
            sz = readlink(filePath.c_str(), buf, size_t(sz));
            if (sz == -1) {
                // Ignore invalid symbolic links
                continue;
            }
            buf[sz]             = '\0';
            std::string newPath = buf;
            if (buf[0] != '/') {
                newPath = fullPath + newPath;
            }
            struct stat st;
            if (stat(newPath.c_str(), &st) == -1 || !S_ISREG(st.st_mode)) {
                // Ignore stat errors and anything else than regular files
                continue;
            }
            if (_filter.has_exclude_content_filters() && exclude_file_by_content(filePath)) {
                continue;
            }
            if (_filter.has_content_filters()) {
                find_in_file(filePath);
            }
            else if (cmd) {
                exec_cmd(*cmd, filePath);
            }
            else {
                fmt::println("{}", filePath);
            }
        }
        else if (DT_DIR == d_type && strcmp(d_name, ".") != 0 && strcmp(d_name, "..") != 0) {
            std::string newPath(path);
            if (!newPath.empty()) {
                newPath.append(1, '/');
            }
            if (_filter.match_file(d_name) && !_filter.has_content_filters() && !cmd) {
                // Directory name itself matches the name filter
                fmt::println("{}{}/ : directory name matches",
                             fullPath,
                             fmt::styled(d_name, nocolor ? fmt::fg(fmt::color{}) : fmt::fg(fmt::color::red)));
            }
            newPath.append(d_name);
            if (_filter.exclude_dir(d_name) || _filter.exclude_dir(newPath)) {
                continue; // Ignore paths that match ignored directory filters
            }
            find_files(root, newPath, dirMatch || _filter.match_dir(d_name) || _filter.match_dir(newPath));
        }
        else if (DT_REG == d_type && _filter.match_file(d_name)) {
            if (!dirMatch) {
                continue;
            }
            std::string const filePath(fullPath + d_name);
            if (_filter.has_exclude_content_filters() && exclude_file_by_content(filePath)) {
                continue;
            }
            if (_filter.has_content_filters()) {
                find_in_file(filePath);
            }
            else if (cmd) {
                exec_cmd(*cmd, filePath);
            }
            else {
                fmt::println("{}{}",
                             fullPath,
                             fmt::styled(d_name, nocolor ? fmt::fg(fmt::color{}) : fmt::fg(fmt::color::red)));
            }
        }
        else if (DT_FIFO == d_type && _filter.match_file(d_name)) {
            if (!dirMatch) {
                continue;
            }
            if (_filter.has_exclude_content_filters() || _filter.has_content_filters()) {
                continue;
            }
            if (cmd) {
                std::string const filePath(fullPath + d_name);
                exec_cmd(*cmd, filePath);
            }
            else {
                fmt::println("{}{}|",
                             fullPath,
                             fmt::styled(d_name, nocolor ? fmt::fg(fmt::color{}) : fmt::fg(fmt::color::red)));
            }
        }
        else if (DT_SOCK == d_type && _filter.match_file(d_name)) {
            if (!dirMatch) {
                continue;
            }
            if (_filter.has_exclude_content_filters() || _filter.has_content_filters()) {
                continue;
            }
            if (cmd) {
                std::string const filePath(fullPath + d_name);
                exec_cmd(*cmd, filePath);
            }
            else {
                fmt::println("{}{}=",
                             fullPath,
                             fmt::styled(d_name, nocolor ? fmt::fg(fmt::color{}) : fmt::fg(fmt::color::red)));
            }
        }
    }
}

/// Returns the type of the inode. Uses stat(2) if the type returned by
/// readdir(3) is unknown.
auto SearchUnix::getType(std::string const &pathname, unsigned char d) -> unsigned char
{
    unsigned char rval = d;
    if (d == DT_UNKNOWN) {
#if defined(FF_AIX)
        struct stat sb;
        if (stat(pathname.c_str(), &sb) == 0) {
            switch (sb.st_type) {
                case VREG:
                    rval = DT_REG;
                    break;
                case VDIR:
                    rval = DT_DIR;
                    break;
                case VLNK:
                    rval = DT_LNK;
                    break;
                case VFIFO:
                    rval = DT_FIFO;
                    break;
                default:;
            }
        }
#else
        struct stat sb;
        if (lstat(pathname.c_str(), &sb) == 0) {
            rval = IFTODT(sb.st_mode);
        }
#endif
    }
    return rval;
}
