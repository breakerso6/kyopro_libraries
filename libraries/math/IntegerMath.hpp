#pragma once
#include <bits/stdc++.h>

namespace integer_math {

template<class T>
T safe_mod(T x, T m) {
    assert(m > 0);
    x %= m;
    if (x < 0) x += m;
    return x;
}

template<class T>
T floor_div(T a, T b) {
    assert(b != 0);
    if (b < 0) a = -a, b = -b;
    if (a >= 0) return a / b;
    return -((-a + b - 1) / b);
}

template<class T>
T ceil_div(T a, T b) {
    assert(b != 0);
    return -floor_div<T>(-a, b);
}

template<class T>
T gcd(T a, T b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) { T r = a % b; a = b; b = r; }
    return a;
}

template<class T>
T lcm(T a, T b) {
    if (a == 0 || b == 0) return 0;
    T result = a / gcd(a, b) * b;
    return result < 0 ? -result : result;
}

struct ExtGCDResult {
    long long gcd, x, y;
};

inline ExtGCDResult ext_gcd(long long a, long long b) {
    long long old_r = a, r = b, old_x = 1, x = 0, old_y = 0, y = 1;
    while (r != 0) {
        long long q = old_r / r;
        std::tie(old_r, r) = std::pair<long long, long long>{r, old_r - q * r};
        std::tie(old_x, x) = std::pair<long long, long long>{x, old_x - q * x};
        std::tie(old_y, y) = std::pair<long long, long long>{y, old_y - q * y};
    }
    if (old_r < 0) old_r = -old_r, old_x = -old_x, old_y = -old_y;
    return {old_r, old_x, old_y};
}

inline long long inv_mod(long long x, long long m) {
    assert(m > 0);
    auto result = ext_gcd(x, m);
    assert(result.gcd == 1);
    return safe_mod(result.x, m);
}

inline long long pow_mod(long long x, long long n, long long m) {
    assert(n >= 0 && m > 0);
    unsigned long long result = 1 % m, base = safe_mod(x, m), mod = m;
    while (n) {
        if (n & 1) result = (unsigned long long)((__uint128_t)result * base % mod);
        base = (unsigned long long)((__uint128_t)base * base % mod);
        n >>= 1;
    }
    return (long long)result;
}

inline std::pair<long long, long long> crt(const std::vector<long long>& r, const std::vector<long long>& m) {
    assert(r.size() == m.size());
    long long r0 = 0, m0 = 1;
    for (int i = 0; i < (int)r.size(); ++i) {
        assert(m[i] >= 1);
        long long r1 = safe_mod(r[i], m[i]), m1 = m[i];
        if (m0 < m1) { std::swap(r0, r1); std::swap(m0, m1); }
        if (m0 % m1 == 0) {
            if (r0 % m1 != r1) return {0, 0};
            continue;
        }
        auto eg = ext_gcd(m0, m1);
        long long g = eg.gcd;
        long long u = eg.x;
        if ((r1 - r0) % g) return {0, 0};
        long long mod = m1 / g;
        long long x = (long long)((__int128)(r1 - r0) / g * u % mod);
        r0 += (long long)((__int128)x * m0);
        m0 *= mod;
        r0 = safe_mod(r0, m0);
    }
    return {r0, m0};
}

inline long long floor_sum(long long n, long long m, long long a, long long b) {
    assert(0 <= n && 1 <= m);
    long long answer = 0;
    if (a < 0) {
        long long a2 = safe_mod(a, m);
        answer -= (__int128)n * (n - 1) / 2 * ((a2 - a) / m);
        a = a2;
    }
    if (b < 0) {
        long long b2 = safe_mod(b, m);
        answer -= (__int128)n * ((b2 - b) / m);
        b = b2;
    }
    while (true) {
        if (a >= m) {
            answer += (__int128)(n - 1) * n * (a / m) / 2;
            a %= m;
        }
        if (b >= m) {
            answer += n * (b / m);
            b %= m;
        }
        long long y_max = ((__int128)a * n + b) / m;
        if (y_max == 0) return answer;
        long long x_max = y_max * m - b;
        answer += (n - (x_max + a - 1) / a) * y_max;
        n = y_max;
        b = (a - x_max % a) % a;
        std::swap(m, a);
    }
}

}  // namespace integer_math
