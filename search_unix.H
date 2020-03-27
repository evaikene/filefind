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

    static void closedir(DIR * d);

    void findFiles(std::string const & root, std::string const & path, bool dirMatch) const override;

    void execCmd(std::string const & cmd, std::string const & path) const override;

    unsigned char getType(std::string const & pathname, unsigned char const d) const;

};

#endif
