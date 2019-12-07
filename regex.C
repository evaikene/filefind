#include "regex.H"
#include "args.H"
#include "error.H"

#include <stddef.h>
#include <string.h>

Regex::Regex(String const & r)
    : _valid(false)
{
	try {
		std::regex::flag_type flags = std::regex::extended;
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
			rval = std::regex_match(s, *pmatch, _preg);
		}
		else {
			rval = std::regex_match(s, _preg);
		}
    }
    return rval;
}
