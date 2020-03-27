#ifndef SEARCH_UNIX_H
#define SEARCH_UNIX_H

#include "search.H"

class SearchWin32 : public Search
{
public:

    SearchWin32(Args const & args);

    ~SearchWin32() override;

protected:

    void findFiles(std::string const & root, std::string const & path, bool dirMatch) const override;

    void execCmd(std::string const & cmd, std::string const & path) const override;

    void printError(std::string const & what) const;

};

#endif
