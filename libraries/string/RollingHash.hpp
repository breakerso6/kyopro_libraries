#pragma once
#include <bits/stdc++.h>

struct RollingHash {
    using ull = unsigned long long;
    static constexpr ull MOD = (1ULL << 61) - 1;
    inline static ull base = 1000003;
    inline static std::vector<ull> powers{1};
    std::vector<ull> prefix;

    RollingHash() = default;
    explicit RollingHash(std::string_view s) : prefix(s.size() + 1) {
        ensure_powers(s.size());
        for (int i = 0; i < (int)s.size(); ++i) prefix[i + 1] = add(mul(prefix[i], base), (unsigned char)s[i] + 1);
    }
    ull hash(int l, int r) const {
        assert(0 <= l && l <= r && r < (int)prefix.size());
        return add(prefix[r], MOD - mul(prefix[l], powers[r - l]));
    }
    static ull concat(ull left, ull right, int right_length) {
        ensure_powers(right_length);
        return add(mul(left, powers[right_length]), right);
    }
private:
    static ull add(ull a, ull b) { a += b; return a >= MOD ? a - MOD : a; }
    static ull mul(ull a, ull b) {
        __uint128_t c = (__uint128_t)a * b;
        ull x = (ull)c & MOD, y = (ull)(c >> 61);
        ull result = x + y;
        if (result >= MOD) result -= MOD;
        return result;
    }
    static void ensure_powers(int n) {
        while ((int)powers.size() <= n) powers.push_back(mul(powers.back(), base));
    }
};
