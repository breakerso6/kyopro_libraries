#pragma once
#include <bits/stdc++.h>

inline std::vector<int> mobius_sieve(int n) {
    std::vector<int> mu(n + 1), primes;
    std::vector<bool> composite(n + 1);
    mu[1] = 1;
    for (int i = 2; i <= n; ++i) {
        if (!composite[i]) primes.push_back(i), mu[i] = -1;
        for (int p : primes) {
            if ((long long)i * p > n) break;
            composite[i * p] = true;
            if (i % p == 0) { mu[i * p] = 0; break; }
            mu[i * p] = -mu[i];
        }
    }
    if (n >= 1) mu[1] = 1;
    return mu;
}

template<class T>
std::vector<T> divisor_zeta(std::vector<T> f) {
    int n = (int)f.size() - 1;
    for (int d = 1; d <= n; ++d) for (int m = d + d; m <= n; m += d) f[m] += f[d];
    return f;
}

template<class T>
std::vector<T> divisor_mobius(std::vector<T> f) {
    int n = (int)f.size() - 1;
    for (int d = n; d >= 1; --d) for (int m = d + d; m <= n; m += d) f[m] -= f[d];
    return f;
}

template<class T>
std::vector<T> multiple_zeta(std::vector<T> f) {
    int n = (int)f.size() - 1;
    for (int d = n; d >= 1; --d) for (int m = d + d; m <= n; m += d) f[d] += f[m];
    return f;
}

template<class T>
std::vector<T> multiple_mobius(std::vector<T> f) {
    int n = (int)f.size() - 1;
    for (int d = 1; d <= n; ++d) for (int m = d + d; m <= n; m += d) f[d] -= f[m];
    return f;
}
