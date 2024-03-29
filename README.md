# filefind

A simple command line tool that combines basic functionality of find and grep.

# Usage

`filefind [args] [path]`

where `path` is an optional path to start searching. Uses the current working directory if no `path` is given. Without any filter arguments, uses `path` as the name filter and searches for it in the current directory.

```
args:
  -a, --all             print all the matching lines in a file
  -A, --ascii           treat all files as ASCII files (no binary file detection)
  -c, --content <regex> file content filter (case sensitive)
  -C, --icontent <regex> file content filter (case insensitive)
  -d, --dir <pattern>   directory name filter (case sensitive)
  -D, --idir <pattern>  directory name filter (case insensitive)
  -e, --extra <n>       print additional <n> lines after a match with -a
  -X, --exec "<cmd> {}" execute <cmd> for every matching file
                        {} will be replaced with the name of the file
  -f, --name <pattern>  file name filter (case sensitive)
  -F, --iname <pattern> file name filter (case insensitive)
  -g, --grammar <name>  Regular expressions grammar (default is extended POSIX grammar)
                        Other options are:
                           ECMAScript - EXMAScript grammar
                           basic - Basic POSIX grammar
                           awk - Awk POSIX grammar
                           grep - Grep POSIX grammar
                           egrep - Egrep POSIX grammar
  -h, --help            print this help, then exit
  -n, --not             prefix for the next file name, directory name,
                        or file content filter making it an exclude filter
  -o, --nocolor         do not highlight search results with colors
                        useful when the search results is used as an input
                        for some other commands
  -v, --version         print version number, then exit
```

File and directory name filters use the fnmatch(3) shell wildcard patterns on unix-like operating systems and PathMatchSpecA() on Windows. File content   filters use regular expressions. By default, the extended POSIX grammar is used, which can be changed with the --grammar command line argument or `[grammar]` section in the configuration file.

Exclude filters exclude directories, file names or content from the subset of files that matches include filters.

The `--all` option allows printing all the matching lines in a file with the matching line number and content. Can only be used if the content filter is not empty and exclude content filter is empty.

Filters can be prefixed with the `--not` argument to make them exclude filters. The same can be achieved by prefixing the filter string itself with `'!'`

File name filters can be built using predefined lists in a configuration file. These start with `'@'` followed by a name of the list. For example, the following configuration file section defines a list of C++ source files:

```
[@cpp]
*.cpp
*.h
*.C
*.H
```

# Configuration files

The application tries to use the user's configuration file "~/.config/filefind". If this is not found, tries to open the global configuration file "/etc/filefind". To use a specific configuration file, specify the full path of the file in the
FILEFIND\_CONFIG environment variable.

The configuration file is a simple ini-like file with sections and values, where every value is on a separate line. Empty lines and lines startng with `'#'` or `';'` are ignored.

The configuration file can be used to defined globally included or exluded directory names in the `[dirs]` section and globally included or excluded file names in the `[files]` section. For example, the following `[dirs]` section would always ignore directories `.git` and `.svn`:

```
[dirs]
!.git
!.svn
```

The `[grammar]` section can be used to defined the default regular expressions grammar. For example, the following `[grammar]` section defines basic POSIX grammar as the default regular expression grammar:

```
[grammar]
basic
```

Additional sections in the configuration file are used to define lists that can be used with `--[i]name`, `--[i]dir` and `--[i]content` command line parameters.

# Platforms

The filefind tool has been sucessfully built on Linux, Mac OS, Widnows, and IBM PASE for i. Note that on IBM PASE for i the fnmatch(3) function does not support case insensitive matching and therefore all the file and directory filters are always case sensitive. Note that on Windows the PathMatchSpecA() function does not support case sensitive matching and therefore all the file and directory filters are always case insensitive.

# Examples

Search for "\*.cpp" and "\*.h" files in the directory "~/src/" containing the string
"MConfig" excluding directories with the names that contain the strings "unit-tests"
or "unittest":

```sh
> filefind ~/src/ --name "*.cpp" --name "*.h" --content "MConfig" --not --dir "unit-tests" --not --dir "unittest"
> filefind ~/src/ --name "@cpp" --content "MConfig" --not --dir "unit-tests" --not --dir "unittest"
> filefind ~/src/ -f "*.cpp" -f "*.h" -c "MConfig" -d '!unit-tests' -d '!unittest'
```

Delete all the "Makefile.in" files in the current directory:

```sh
> filefind -f "Makefile.in" -X "rm {}"
```

Use a custom configuration file to search for all the "\*.pacnew" files in the "/etc" directory:

```sh
> cat > ~/.config/filefind_pacman << EOF
[dirs]
!.git
[files]
*.pacnew
EOF
> FILEFIND_CONFIG=~/.config/filefind_pacman filefind /etc
```

# Building

Building filefind requires cmake. Create a build directory and run:

```sh
> cmake <path-to-filefind>
> make
> make install
```

# Building with cmake presets and vcpkq (requires ninja)

```sh
> cmake --preset=ninja-multi
> cmake --build --preset=ninja-release
> sudo cmake --install builds/ninja-multi
```

# Building with GNU autotools

Requires GNU autotools to be installed. Run the following commands in the source directory:

```sh
> autoreconf --install
> ./configure
> make
> make install
```
