#ifndef UTILS_H_INCLUDED
#  define UTILS_H_INCLUDED

#include <memory>
#include <optional>
#include <string>

#include <stdio.h>

namespace Utils {

/// Return a system error message
/// @param[in] errnum Error number
/// @return Error message as a string
auto strerror(int errnum) -> std::string;

/// Get an environment variable
/// @param[in] name Name of the variable
/// @return Environment value or an invalid optional
auto getenv(char const *name) -> std::optional<std::string>;

using FilePtr = std::unique_ptr<FILE, void (*)(FILE *)>;

/// Returns an invalid file pointer
/// @return Invalid file pointer
auto InvalidFilePtr() -> FilePtr;

/// Open a file stream
/// @param[in] path Name of the file
/// @param[in] mode Opening mode
/// @return Pointer to the file stream when successful, nullptr otherwise
auto fopen(std::string const &path, std::string const &mode = "r") -> FilePtr;

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
auto strncpy_s(char *dst, size_t sz, char const *src, size_t len) -> char *;

} // namespace Utils

#endif
