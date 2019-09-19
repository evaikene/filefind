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
  -E, --exec "<cmd> {}" execute <cmd> for every matching file
                        {} will be replaced with the name of the file
  -f, --name <pattern>  file name filter (case sensitive)
  -F, --iname <pattern> file name filter (case insensitive)
  -h, --help            prints this help message and exits
  -n, --not             prefix for the next file name, directory name,
                        or file content filter making it an exclude filter
  -o, --nocolor         do not highlight search results with colors
                        useful when the search results is used as an input
                        for some other commands
```

File and directory name filters use the fnmatch(3) shell wildcard patterns. File content filters use regex(7) regular expressions.

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

The application tries to use the user's configuration file "~/.config/filefind". If this is not found, tries to open the global configuration file "/etc/filefind".

The configuration file is a simple ini-like file with sections and values, where every value is on a separate line. Empty lines and lines startng with `'#'` or `';'` are ignored.

The configuration file can be used to defined globally included or exluded directory names in the `[dirs]` section and globally included or excluded file names in the `[files]` section. For example, the following `[dirs]` section would always ignore directories `.git` and `.svn`:
```
[dirs]
!.git
!.svn
```
Additional sections in the configuration file are used to define lists that can be used with `--[i]name`, `--[i]dir` and `--[i]content` command line parameters.

# Examples

Search for "\*.cpp" and "\*.h" files in the directory "~/src/" containing the string
"MConfig" excluding directories with the names that contain the strings "unit-tests"
or "unittest"

```
> filefind ~/src/ --name "*.cpp" --name "*.h" --content "MConfig" --not --dir "unit-tests" --not --dir "unittest"
> filefind ~/src/ --name "@cpp" --content "MConfig" --not --dir "unit-tests" --not --dir "unittest"
> filefind ~/src/ -f "*.cpp" -f "*.h" -c "MConfig" -d '!unit-tests' -d '!unittest'
```

# Building
Building filefind requires cmake and boost. Create a build directory and run:
```
> cmake <path-to-filefind>
> make
```
