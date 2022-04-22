#ifndef REGEX_H
#define REGEX_H

#if defined(RE2_FOUND)
# include "re2_regex.H"
#else
# include "std_regex.H"
#endif

#endif // REGEX_H
