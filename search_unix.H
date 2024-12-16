#ifndef SEARCH_UNIX_H_INCLUDED
#  define SEARCH_UNIX_H_INCLUDED

#include "search.H"

class SearchUnix : public Search
{
public:

    SearchUnix(Args const & args);

    ~SearchUnix() override;

protected:

    static auto getType(std::string const &pathname, unsigned char d) -> unsigned char;

    void find_files(std::string const &root, std::string const &path, bool dirMatch) const override;

    void exec_cmd(std::string const &cmd, std::string const &path) const override;

};

#endif
