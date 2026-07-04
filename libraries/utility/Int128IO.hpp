#pragma once
#include <bits/stdc++.h>

namespace int128_io_detail {
inline std::string to_string_unsigned(unsigned __int128 value) {
    if (value == 0) return "0";
    std::string s;
    while (value > 0) {
        s.push_back((char)('0' + value % 10));
        value /= 10;
    }
    std::reverse(s.begin(), s.end());
    return s;
}

inline bool parse_unsigned(const std::string& s, unsigned __int128& value, bool allow_plus) {
    int pos = 0;
    if (allow_plus && pos < (int)s.size() && s[pos] == '+') ++pos;
    if (pos == (int)s.size()) return false;
    value = 0;
    unsigned __int128 limit = ~((unsigned __int128)0);
    for (; pos < (int)s.size(); ++pos) {
        if (!std::isdigit((unsigned char)s[pos])) return false;
        int digit = s[pos] - '0';
        if (value > (limit - digit) / 10) return false;
        value = value * 10 + digit;
    }
    return true;
}
} // namespace int128_io_detail

inline std::ostream& operator<<(std::ostream& os, unsigned __int128 value) {
    return os << int128_io_detail::to_string_unsigned(value);
}

inline std::ostream& operator<<(std::ostream& os, __int128_t value) {
    if (value < 0) {
        os << '-';
        unsigned __int128 magnitude = (unsigned __int128)(-(value + 1)) + 1;
        return os << int128_io_detail::to_string_unsigned(magnitude);
    }
    return os << int128_io_detail::to_string_unsigned((unsigned __int128)value);
}

inline std::istream& operator>>(std::istream& is, unsigned __int128& value) {
    std::string s;
    is >> s;
    unsigned __int128 parsed = 0;
    if (!is || !int128_io_detail::parse_unsigned(s, parsed, true)) {
        is.setstate(std::ios::failbit);
        return is;
    }
    value = parsed;
    return is;
}

inline std::istream& operator>>(std::istream& is, __int128_t& value) {
    std::string s;
    is >> s;
    if (!is || s.empty()) {
        is.setstate(std::ios::failbit);
        return is;
    }
    bool negative = false;
    if (s[0] == '-' || s[0] == '+') {
        negative = s[0] == '-';
        s.erase(s.begin());
    }
    unsigned __int128 parsed = 0;
    if (!int128_io_detail::parse_unsigned(s, parsed, false)) {
        is.setstate(std::ios::failbit);
        return is;
    }
    unsigned __int128 positive_limit = (((unsigned __int128)1) << 127) - 1;
    unsigned __int128 negative_limit = ((unsigned __int128)1) << 127;
    if ((!negative && parsed > positive_limit) || (negative && parsed > negative_limit)) {
        is.setstate(std::ios::failbit);
        return is;
    }
    if (negative) value = parsed == negative_limit ? -(__int128_t)(negative_limit - 1) - 1 : -(__int128_t)parsed;
    else value = (__int128_t)parsed;
    return is;
}
