#pragma once
#include <bits/stdc++.h>

template<class Mint>
struct ModCombinatorics {
    std::vector<Mint> fact{Mint(1)}, inv_fact{Mint(1)}, inv{Mint(0)}, der{Mint(1)};

    ModCombinatorics() = default;
    explicit ModCombinatorics(int n) { ensure(n); }

    void ensure(int n) {
        assert(n >= 0);
        int old = (int)fact.size() - 1;
        if (n <= old) return;
        fact.resize(n + 1);
        for (int i = old + 1; i <= n; ++i) fact[i] = fact[i - 1] * Mint(i);
        inv_fact.resize(n + 1);
        inv_fact[n] = Mint(1) / fact[n];
        for (int i = n; i > old + 1; --i) inv_fact[i - 1] = inv_fact[i] * Mint(i);
        inv.resize(n + 1);
        for (int i = old + 1; i <= n; ++i) inv[i] = inv_fact[i] * fact[i - 1];
    }

    void ensure_derangement(int n) {
        assert(n >= 0);
        int old = (int)der.size() - 1;
        if (n <= old) return;
        der.resize(n + 1);
        if (old == 0 && n >= 1) der[1] = Mint(0), old = 1;
        for (int i = old + 1; i <= n; ++i) der[i] = Mint(i - 1) * (der[i - 1] + der[i - 2]);
    }

    Mint factorial(int n) {
        assert(n >= 0);
        ensure(n);
        return fact[n];
    }
    Mint inv_factorial(int n) {
        assert(n >= 0);
        ensure(n);
        return inv_fact[n];
    }
    Mint inverse(int n) {
        assert(n >= 1);
        ensure(n);
        return inv[n];
    }
    Mint perm(int n, int k) {
        if (k < 0 || n < k) return Mint(0);
        ensure(n);
        return fact[n] * inv_fact[n - k];
    }
    Mint comb(int n, int k) {
        if (k < 0 || n < k) return Mint(0);
        ensure(n);
        return fact[n] * inv_fact[k] * inv_fact[n - k];
    }
    Mint homo(int n, int k) {
        if (n < 0 || k < 0) return Mint(0);
        if (n == 0) return k == 0 ? Mint(1) : Mint(0);
        return comb(n + k - 1, k);
    }
    Mint multinomial(const std::vector<int>& counts) {
        int total = 0;
        for (int count : counts) {
            if (count < 0) return Mint(0);
            total += count;
        }
        ensure(total);
        Mint result = fact[total];
        for (int count : counts) result *= inv_fact[count];
        return result;
    }
    Mint multinomial(std::initializer_list<int> counts) {
        return multinomial(std::vector<int>(counts));
    }
    Mint multinomial(int n, const std::vector<int>& counts) {
        if (n < 0) return Mint(0);
        int total = 0;
        for (int count : counts) {
            if (count < 0) return Mint(0);
            total += count;
        }
        if (total != n) return Mint(0);
        return multinomial(counts);
    }
    Mint catalan(int n) {
        if (n < 0) return Mint(0);
        return comb(2 * n, n) * inverse(n + 1);
    }
    Mint derangement(int n) {
        if (n < 0) return Mint(0);
        ensure_derangement(n);
        return der[n];
    }
};
