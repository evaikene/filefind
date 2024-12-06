#ifndef SEARCH_H_INCLUDED
#  define SEARCH_H_INCLUDED

#include "filter.H"

#include <stdio.h>

#include <string>

class Args;

/// Generic file search class
class Search {
public:

    /// Returns the platform-specific instance
    static Search &instance(Args const &args);

    /// Destroys the global instance
    static void destroyInstance();

    /// Disabled default constructor
    Search() = delete;

    /// Disabled copy constructor
    Search(Search const &) = delete;

    /// Disabled move constructor
    Search(Search &&) = delete;

    /// Disabled assignment operators
    Search &operator=(Search const &) = delete;
    Search &operator=(Search &&)      = delete;

    /// Performs the search
    /// @param[in] args Command line arguments
    void search() const;

protected:

    Args const &_args;

    Filter _filter;

    /// Constructor
    explicit Search(Args const &args);

    /// Destructor
    virtual ~Search();

    bool exclude_file_by_content(std::string const &path) const;

    void find_in_file(std::string const &path) const;

    virtual size_t print_match(char const *buf, Match const &pmatch, bool nocolor) const;

    virtual void find_files(std::string const &root, std::string const &path, bool dirMatch) const = 0;

    virtual void exec_cmd(std::string const &cmd, std::string const &path) const = 0;

private:

    /// Global instance
    static Search *_instance;
};

#endif
