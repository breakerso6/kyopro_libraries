#pragma once
#include <bits/stdc++.h>

template<class T>
struct MappingTwelvefold {
    int max_n;
    std::vector<std::vector<T>> comb, stirling2, partition_exact;

    explicit MappingTwelvefold(int max_n_) : max_n(max_n_) {
        comb.assign(max_n + 1, std::vector<T>(max_n + 1));
        stirling2.assign(max_n + 1, std::vector<T>(max_n + 1));
        partition_exact.assign(max_n + 1, std::vector<T>(max_n + 1));
        for (int n = 0; n <= max_n; ++n) {
            comb[n][0] = comb[n][n] = 1;
            for (int k = 1; k < n; ++k) comb[n][k] = comb[n - 1][k - 1] + comb[n - 1][k];
        }
        stirling2[0][0] = 1;
        for (int n = 1; n <= max_n; ++n)
            for (int k = 1; k <= n; ++k) stirling2[n][k] = stirling2[n - 1][k - 1] + T(k) * stirling2[n - 1][k];
        partition_exact[0][0] = 1;
        for (int part = 1; part <= max_n; ++part)
            for (int sum = part; sum <= max_n; ++sum)
                for (int k = 1; k <= max_n; ++k) partition_exact[sum][k] += partition_exact[sum - part][k - 1];
    }
    T pow_t(T a, long long e) const {
        T r = 1;
        while (e) { if (e & 1) r *= a; a *= a; e >>= 1; }
        return r;
    }
    T factorial(int n) const { T r = 1; for (int i = 2; i <= n; ++i) r *= i; return r; }

    T distinct_to_distinct_any(int n, int k) const { return pow_t(T(k), n); }
    T distinct_to_distinct_injective(int n, int k) const {
        if (n > k) return 0;
        T r = 1; for (int i = 0; i < n; ++i) r *= k - i; return r;
    }
    T distinct_to_distinct_surjective(int n, int k) const { return 0 <= k && k <= max_n && 0 <= n && n <= max_n ? factorial(k) * stirling2[n][k] : 0; }
    T distinct_to_identical_any(int n, int k) const { return n <= max_n && k <= max_n ? std::accumulate(stirling2[n].begin(), stirling2[n].begin() + std::min(n, k) + 1, T()) : 0; }
    T distinct_to_identical_injective(int n, int k) const { return n <= k ? 1 : 0; }
    T distinct_to_identical_surjective(int n, int k) const { return n <= max_n && k <= max_n ? stirling2[n][k] : 0; }
    T identical_to_distinct_any(int n, int k) const {
        if (n == 0) return 1;
        if (k == 0) return 0;
        return n + k <= max_n ? comb[n + k - 1][k - 1] : 0;
    }
    T identical_to_distinct_injective(int n, int k) const { return n <= k ? comb[k][n] : 0; }
    T identical_to_distinct_surjective(int n, int k) const {
        if (k == 0) return n == 0 ? 1 : 0;
        return n >= k && n <= max_n ? comb[n - 1][k - 1] : 0;
    }
    T identical_to_identical_any(int n, int k) const {
        T r = 0; for (int i = 0; i <= std::min(n, k); ++i) r += partition_exact[n][i]; return r;
    }
    T identical_to_identical_injective(int n, int k) const { return n <= k ? 1 : 0; }
    T identical_to_identical_surjective(int n, int k) const { return n <= max_n && k <= max_n ? partition_exact[n][k] : 0; }
};
