#include "regex.H"
#include "args.H"
#include "error.H"

#include <stddef.h>
#include <string.h>

std::regex::flag_type Regex::grammarFromString(std::string const & grammar)
{
    if (grammar.empty() || grammar == "extended") {
        return std::regex::extended;
    }
    else if (grammar == "ECMAScript") {
        return std::regex::ECMAScript;
    }
    else if (grammar == "basic") {
        return std::regex::basic;
    }
    else if (grammar == "awk") {
        return std::regex::awk;
    }
    else if (grammar == "grep") {
        return std::regex::grep;
    }
    else if (grammar == "egrep") {
        return std::regex::egrep;
    }
    else {
        // Fall-back is extended
        return std::regex::extended;
    }
}

Regex::Regex(String const & r, std::regex::flag_type flags)
    : _valid(false)
{
	try {
		if (r.noCase()) {
			flags |= std::regex::icase;
		}
		_preg = std::regex(r, flags);
		_valid = true;
	}
	catch (std::regex_error const& ex) {
		throw Error(ex.what());
	}
}

Regex::~Regex()
{
}

bool Regex::match(std::string const & s, std::smatch * pmatch) const
{
    bool rval = false;
    if (_valid) {
		if (pmatch != nullptr) {
			rval = std::regex_search(s, *pmatch, _preg);
		}
		else {
			rval = std::regex_search(s, _preg);
		}
    }
    return rval;
}
