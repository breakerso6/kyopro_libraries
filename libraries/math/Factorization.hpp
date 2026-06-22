#pragma once
#include <bits/stdc++.h>

namespace factorization {
using u64 = std::uint64_t;
using u128 = __uint128_t;

inline u64 multiply_mod(u64 a, u64 b, u64 mod) { return (u128)a * b % mod; }
inline u64 power_mod(u64 a, u64 e, u64 mod) {
    u64 result = 1;
    while (e) { if (e & 1) result = multiply_mod(result, a, mod); a = multiply_mod(a, a, mod); e >>= 1; }
    return result;
}
inline bool is_prime(u64 n) {
    if (n < 2) return false;
    for (u64 p : {2ULL, 3ULL, 5ULL, 7ULL, 11ULL, 13ULL, 17ULL, 19ULL, 23ULL, 29ULL, 31ULL, 37ULL}) {
        if (n % p == 0) return n == p;
    }
    u64 d = n - 1, s = 0;
    while ((d & 1) == 0) d >>= 1, ++s;
    for (u64 a : {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL}) {
        if (a % n == 0) continue;
        u64 x = power_mod(a % n, d, n);
        if (x == 1 || x == n - 1) continue;
        bool composite = true;
        for (u64 r = 1; r < s; ++r) {
            x = multiply_mod(x, x, n);
            if (x == n - 1) { composite = false; break; }
        }
        if (composite) return false;
    }
    return true;
}
inline u64 pollard_rho(u64 n) {
    if (n % 2 == 0) return 2;
    static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    while (true) {
        u64 c = std::uniform_int_distribution<u64>(1, n - 1)(rng);
        u64 x = std::uniform_int_distribution<u64>(0, n - 1)(rng), y = x, d = 1;
        auto f = [&](u64 v) { return (u128(multiply_mod(v, v, n)) + c) % n; };
        while (d == 1) {
            x = f(x); y = f(f(y));
            d = std::gcd(x > y ? x - y : y - x, n);
        }
        if (d != n) return d;
    }
}
inline void factor_rec(u64 n, std::vector<u64>& factors) {
    if (n == 1) return;
    if (is_prime(n)) { factors.push_back(n); return; }
    u64 divisor = pollard_rho(n);
    factor_rec(divisor, factors); factor_rec(n / divisor, factors);
}
inline std::vector<u64> prime_factors(u64 n) {
    assert(n >= 1);
    std::vector<u64> result;
    factor_rec(n, result); std::sort(result.begin(), result.end()); return result;
}
inline std::vector<std::pair<u64, int>> factorize(u64 n) {
    assert(n >= 1);
    std::vector<std::pair<u64, int>> result;
    for (u64 p : prime_factors(n)) {
        if (result.empty() || result.back().first != p) result.push_back({p, 1});
        else ++result.back().second;
    }
    return result;
}
inline std::vector<u64> divisors(u64 n) {
    assert(n >= 1);
    std::vector<u64> result{1};
    for (auto [p, exponent] : factorize(n)) {
        int old_size = result.size(); u64 power = 1;
        for (int e = 1; e <= exponent; ++e) {
            power *= p;
            for (int i = 0; i < old_size; ++i) result.push_back(result[i] * power);
        }
    }
    std::sort(result.begin(), result.end()); return result;
}
} // namespace factorization
