#ifndef SEARCH_H
#define SEARCH_H

#include "filter.H"

#include <stdio.h>

#include <string>

class Args;

/// Generic file search class
class Search {
public:

    /// Returns a platform-specific instance
    static Search & instance(Args const & args);

    /// Destroys the global instance
    static void destroyInstance();

    /// Disabled default constructor
    Search() = delete;

    /// Disabled copy constructor
    Search(Search const &) = delete;

    /// Disabled move constructor
    Search(Search &&) = delete;

    /// Disabled assignment operators
    Search & operator=(Search const &) = delete;
    Search & operator=(Search &&) = delete;

    /// Performs the search
    /// @param[in] args Command line arguments
    void search() const;

protected:

    Args const & _args;

    Filter _filter;

    static void fclose(FILE * f);

    /// Constructor
    explicit Search(Args const & args);

    /// Destructor
    virtual ~Search();

    bool excludeFileByContent(std::string const & path) const;

    void findInFile(std::string const & path) const;

    virtual size_t printMatch(char const * buf, Match const & pmatch, bool nocolor) const;

    virtual void findFiles(std::string const & root, std::string const & path, bool dirMatch) const = 0;

    virtual void execCmd(std::string const & cmd, std::string const & path) const = 0;

private:

    /// Global instance
    static Search * _instance;

};

#endif
