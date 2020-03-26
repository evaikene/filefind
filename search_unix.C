#include "search.H"
#include "args.H"
#include "filter.H"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(__APPLE__)
#include <sys/syslimits.h>
#endif

#if defined(_AIX)
// struct dirent on AIX does not have the d_type field nor defines for its values
#include <sys/vnode.h>
#define DT_UNKNOWN 0
#define DT_DIR 2
#define DT_REG 3
#define DT_LNK 10
#endif

namespace
{
	void _closedir(DIR* d)
	{
		if (nullptr != d) {
			closedir(d);
		}
	}

	void _fclose(FILE* f)
	{
		if (nullptr != f) {
			fclose(f);
		}
	}

	/// Executes the command for a file
	void execCmd(std::string const& cmd, std::string const& path)
	{
		std::string cmdline(cmd);
		size_t pos = 0;
		while ((pos = cmdline.find("{}", pos)) != std::string::npos)
		{
			cmdline.replace(pos, 2, path);
			pos = pos - 2 + path.size();
		}
		if (system(cmdline.c_str()) != 0) {}
	}

	bool excludeFileByContent(std::string const& path, Filter const& filter)
	{
		bool rval = false;
		std::unique_ptr<FILE, decltype(&_fclose)> f(fopen(path.c_str(), "r"), &_fclose);
		if (!f) {
			fprintf(stderr, "\033[31mERROR:\033[0m Failed to open file %s : %s\n", path.c_str(), strerror(errno));
			return rval;
		}

		char buf[1024];
		while (!rval && fgets(buf, sizeof(buf), f.get()) != nullptr) {
			// Remove trailing CR and LF characters
			size_t sz = strlen(buf);
			while (sz > 0 && (buf[sz - 1] == '\n' || buf[sz - 1] == '\r')) {
				buf[--sz] = '\0';
			}
			rval = filter.excludeContent(buf);
		}
		return rval;
	}

    size_t const BUF_SIZE = 1024;

    size_t printMatch(char const * buf, std::smatch const & pmatch, bool nocolor)
    {
        size_t const sz = strlen(buf);
        char s1[BUF_SIZE] = "";
        char s2[BUF_SIZE] = "";
        size_t msz = std::min<size_t>(pmatch.position(), BUF_SIZE - 1);
        strncpy(s1, buf, msz);
        s1[msz] = '\0';
        size_t idx = msz;
        if (idx < sz) {
            msz = std::min<size_t>(pmatch.length(), BUF_SIZE - 1);
            strncpy(s2, &buf[idx], msz);
            s2[msz] = '\0';
            idx += msz;
        }

        printf("%s%s%s%s",
            s1,
            nocolor ? "" : "\033[31m",
            s2,
            nocolor ? "" : "\033[0m");

        return idx;
    }

	void findInFile(std::string const& path, Filter const& filter)
	{
		std::unique_ptr<FILE, decltype(&_fclose)> f(fopen(path.c_str(), "r"), &_fclose);
		if (!f) {
			fprintf(stderr, "\033[31mERROR:\033[0m Failed to open file %s : %s\n", path.c_str(), strerror(errno));
			return;
		}

		char buf[BUF_SIZE];
		int lineno = 0;
		bool binary = false;
		int linesToPrint = 0;
		bool const nocolor = filter.args().noColor();
		while (fgets(buf, BUF_SIZE, f.get()) != nullptr) {
			// Remove trailing CR and LF characters
			size_t sz = strlen(buf);
			while (sz > 0 && (buf[sz - 1] == '\n' || buf[sz - 1] == '\r')) {
				buf[--sz] = '\0';
			}

			// Check for a binary file
			for (size_t i = 0; !filter.args().ascii() && !binary && i < sz; ++i) {
				binary = (isprint(buf[i]) == 0 && buf[i] != '\t');
			}

			++lineno;
			std::smatch pmatch;
			if (filter.matchContent(buf, &pmatch)) {
				if (filter.printContent()) {
					if (!binary) {

                        printf("%s +%d : \"", path.c_str(), lineno);

						// Print content
                        size_t idx = printMatch(buf, pmatch, nocolor);

                        // Repeat search for more matches
                        while (idx < sz && filter.matchContent(&buf[idx], &pmatch)) {
                            idx += printMatch(&buf[idx], pmatch, nocolor);
                        }

                        // The remainder of the line
                        if (idx < sz) {
                            char s3[BUF_SIZE];
                            strncpy(s3, &buf[idx], BUF_SIZE - 1);
                            s3[BUF_SIZE - 1] = '\0';
                            printf("%s\"\n", s3);
                        }
                        else {
                            printf("\"\n");
                        }

						linesToPrint = filter.args().extraContent();
					}
					else {
						// Print only file name and exit
						printf("%s : binary file matches\n", path.c_str());
						break;
					}
				}
				else if (!filter.args().execCmd().empty()) {
					// Run the command and exit
					execCmd(filter.args().execCmd(), path);
					break;
				}
				else {
					// Print only file name and exit
					printf("%s\n", path.c_str());
					break;
				}
			}

			// Print extra content
			if (linesToPrint > 0) {
				printf("\t%s\n", buf);
				--linesToPrint;
			}
		}
	}

	/// Returns the type of the inode. Uses stat(2) if the type returned by
	/// readdir(3) is unknown.
	unsigned char getType(std::string const& pathname, unsigned char const d)
	{
		unsigned char rval = d;
		if (d == DT_UNKNOWN) {
#if defined(_AIX)
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

	void findFiles(std::string const& root, std::string const& path, bool dirMatch, Filter const& filter)
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
		std::unique_ptr<DIR, decltype(&_closedir)> dir(opendir(fullPath.c_str()), &_closedir);
		if (!dir) {
			fprintf(stderr, "\033[31mERROR:\033[0m Failed to open directory %s : %s\n", fullPath.c_str(), strerror(errno));
			return;
		}

		std::string const cmd(filter.args().execCmd());
		bool const hasCmd(!cmd.empty());

		bool const nocolor = filter.args().noColor();
		struct dirent const* dent = nullptr;
		while ((dent = readdir(dir.get())) != nullptr) {
			char const* d_name = dent->d_name;
#if defined(_AIX)
			unsigned char const d_type = getType(fullPath + d_name, DT_UNKNOWN);
#else
			unsigned char const d_type = getType(fullPath + d_name, dent->d_type);
#endif

			if (DT_LNK == d_type) {
				if (!filter.matchFile(d_name)) {
					// Skip symbolic links that do not match the file name filter
					continue;
				}
				std::string const filePath(fullPath + d_name);
				char buf[PATH_MAX];
				ssize_t sz = PATH_MAX - 1;
				if ((sz = readlink(filePath.c_str(), buf, size_t(sz))) == -1) {
					// Ignore invalid symbolic links
					continue;
				}
				buf[sz] = '\0';
				std::string newPath = buf;
				if (buf[0] != '/') {
					newPath = fullPath + newPath;
				}
				struct stat st;
				if (stat(newPath.c_str(), &st) == -1 || !S_ISREG(st.st_mode)) {
					// Ignore stat errors and anything else than regular files
					continue;
				}
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
					printf("%s\n", filePath.c_str());
				}
			}
			else if (DT_DIR == d_type && strcmp(d_name, ".") != 0
				&& strcmp(d_name, "..") != 0) {
				std::string newPath(path);
				if (!newPath.empty()) {
					newPath.append(1, '/');
				}
				if (filter.matchFile(d_name) && !filter.hasContentFilters() && filter.args().execCmd().empty()) {
					// Directory name itself matches the name filter
					printf("%s%s%s%s/ : directory name matches\n",
						fullPath.c_str(),
						nocolor ? "" : "\033[31m",
						d_name,
						nocolor ? "" : "\033[0m");
				}
				newPath.append(d_name);
				if (filter.excludeDir(d_name) || filter.excludeDir(newPath)) {
					continue; // Ignore paths that match ignored directory filters
				}
				findFiles(root, newPath, dirMatch | filter.matchDir(d_name) | filter.matchDir(newPath), filter);
			}
			else if (DT_REG == d_type && filter.matchFile(d_name)) {
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
					printf("%s%s%s%s\n",
						fullPath.c_str(),
						nocolor ? "" : "\033[31m",
						d_name,
						nocolor ? "" : "\033[0m");
				}
			}
		}
	}

}

void search(Args const& args)
{
	Filter f(args);

	// Recursively search for files
	findFiles(args.path(), "", f.matchDir(""), f);
}