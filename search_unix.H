#ifndef SEARCH_UNIX_H
#define SEARCH_UNIX_H

#include "search.H"

#include <dirent.h>

class SearchUnix : public Search
{
public:

    SearchUnix(Args const & args);

    ~SearchUnix() override;

protected:

    void find_files(std::string const & root, std::string const & path, bool dirMatch) const override;

    void exec_cmd(std::string const & cmd, std::string const & path) const override;

    unsigned char getType(std::string const & pathname, unsigned char const d) const;

};

#endif
