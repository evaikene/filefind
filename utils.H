#ifndef UTILS_H
#define UTILS_H

#include <string>

#include <stdio.h>

namespace Utils
{

    /// Return a system error message
    /// @param[in] errnum Error numner
    /// @return Error message
    std::string strerror(int errnum);

    /// Get an environment variable
    /// @param[in] name Name of the variable
    /// @return Environment value or an empty string if not found
    std::string getenv(std::string const & name);

    /// Open a file stream
    /// @param[in] path Name of the file
    /// @param[in] mode Opening mode
    /// @return File stream or nullptr if failed
    FILE * fopen(std::string const & path, std::string const & mode = "r");

    /// Copy strings
    /// @param[in] dst Pointer to the character array to copy to
    /// @param[in] sz Size of the destination buffer
    /// @param[in] src Pointer to the character array ti copy from
    /// @param[in] len Maximum number of characters to copy
    /// @return Returns @p dst
    ///
    /// Copies at most @p len characters of the character array pointed to by @p src
    /// to the character array pointed to by @p dst, stopping at the first null character.
    /// Adds a null terminator, thus resulting in a null terminated string.
    ///
    /// Size of the destination character array specified in @p sz shall be at least
    /// @p len + 1. If @p sz is less than @p len + 1, copies at most @p sz - 1 characters.
    char * strncpy_s(char * dst, size_t sz, char const * src, size_t len);

}

#endif
