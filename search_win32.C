#include "search.H"
#include "args.H"
#include "filter.H"

#include <Windows.h>
#include <strsafe.h>

namespace
{
    /**
     * Prints an error message to the stderr
     * @param[in] what Name of the function that failed
     */
    void printError(std::string const & what)
    {
        DWORD const dw = GetLastError();
        LPTSTR lpMsgBuf = nullptr;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            lpMsgBuf,
            0, nullptr);
        LPTSTR lpDisplayBuf = (LPSTR)LocalAlloc(LMEM_ZEROINIT,
            (lstrlen(lpMsgBuf) + what.size() + 40) * sizeof(TCHAR));
        StringCchPrintf(lpDisplayBuf,
            LocalSize(lpDisplayBuf) / sizeof(TCHAR),
            TEXT("%s failed with error %d: %s"),
            what.c_str(), dw, lpMsgBuf);
        fprintf(stderr, "%s\n", (char *)lpDisplayBuf);

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
    }

    void execCmd(std::string const& cmd, std::string const& path)
    {}

    bool excludeFileByContent(std::string const& path, Filter const& filter)
    {
        return false;
    }

    void findInFile(std::string const& path, Filter const& filter)
    {}

    void findFiles(std::string const& root, std::string const& path, bool dirMatch, Filter const& filter)
    {
        std::string fullPath(root);
        if (!fullPath.empty() && fullPath.at(fullPath.size() - 1) != '\\') {
            fullPath.append(1, '\\');
        }
        if (!path.empty()) {
            fullPath.append(path);
            if (path.at(path.size() - 1) != '\\') {
                fullPath.append(1, '\\');
            }
        }

        std::string const cmd(filter.args().execCmd());
        bool const hasCmd(!cmd.empty());

        HANDLE hFind;
        WIN32_FIND_DATA fileData;
        std::string const pattern(fullPath + "*");
        if ((hFind = FindFirstFile(pattern.c_str(), &fileData)) == INVALID_HANDLE_VALUE) {
            printError("FindFirstFile");
            return;
        }
        do {
            char const * d_name = fileData.cFileName;
            if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    && strcmp(d_name, ".") != 0 && strcmp(d_name, "..") != 0) {
                std::string newPath(path);
                if (!newPath.empty()) {
                    newPath.append(1, '\\');
                }
                if (filter.matchFile(d_name) && !filter.hasContentFilters() && filter.args().execCmd().empty()) {
                    // Directory name itself matches the name filter
                    printf("%s%s : directory name matches\n",
                        fullPath.c_str(), d_name);
                }
                newPath.append(d_name);
                if (filter.excludeDir(d_name) || filter.excludeDir(newPath)) {
                    continue; // Ignore paths that match ignored directory filters
                }
                findFiles(root, newPath, dirMatch | filter.matchDir(d_name) | filter.matchDir(newPath), filter);
            }
            else if (filter.matchFile(d_name)) {
                if (!dirMatch) {
                    continue;
                }
                std::string const filePath(fullPath + d_name);
                if (filter.hasExcludeContentFilters() && excludeFileByContent(filePath, filter)) {
                    continue;
                }
                if (filter.hasContentFilters()) {
                    findInFile(filePath, filter);
                }
                else if (hasCmd) {
                    execCmd(cmd, filePath);
                }
                else {
                    printf("%s%s\n", fullPath.c_str(), d_name);
                }
            }
        } while (FindNextFile(hFind, &fileData));

        if (GetLastError() != ERROR_NO_MORE_FILES) {
            printError("FindNextFile");
        }

        FindClose(hFind);
    }
}

void search(Args const& args)
{
    Filter f(args);

    // Recursively search for files
    findFiles(args.path(), "", f.matchDir(""), f);
}
