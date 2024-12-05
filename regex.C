#include "args.H"
#include "error.H"
#include "regex.H"

#include <re2/re2.h>

// -----------------------------------------------------------------------------

Regex::Regex(String const &r)
{
    // Compile regex
    re2::RE2::Options opts;
    opts.set_case_sensitive(!r.noCase());
    opts.set_log_errors(false);
    _rx    = std::make_unique<re2::RE2>(r, opts);
    _valid = _rx->ok();
    if (!_valid) {
        throw Error{_rx->error()};
    }
}

Regex::~Regex() = default;

bool Regex::match(std::string_view s, Match *pmatch) const
{
    if (!_rx)
        return false;
    re2::StringPiece const str{s.data(), s.size()};
    re2::StringPiece       substr;
    auto const             result = _rx->Match(str, 0, str.size(), re2::RE2::UNANCHORED, &substr, 1);
    if (result && pmatch) {
        pmatch->set_pos_and_len(substr.data() - str.data(), substr.size());
    }
    return result;
}
