#pragma once
#include <bits/stdc++.h>

template<class T>
struct WaveletMatrix {
    int n = 0, levels = 0;
    std::vector<T> values;
    std::vector<std::vector<int>> prefix_one;
    std::vector<int> zero_count;

    WaveletMatrix() = default;
    explicit WaveletMatrix(const std::vector<T>& a) { build(a); }

    void build(const std::vector<T>& a) {
        n = (int)a.size();
        values = a;
        std::sort(values.begin(), values.end());
        values.erase(std::unique(values.begin(), values.end()), values.end());
        levels = 0;
        while ((1ULL << levels) < std::max<size_t>(1, values.size())) ++levels;
        levels = std::max(levels, 1);
        prefix_one.assign(levels, std::vector<int>(n + 1));
        zero_count.assign(levels, 0);
        std::vector<int> cur(n), next(n);
        for (int i = 0; i < n; ++i)
            cur[i] = std::lower_bound(values.begin(), values.end(), a[i]) - values.begin();
        for (int level = 0; level < levels; ++level) {
            int bit = levels - 1 - level;
            for (int i = 0; i < n; ++i)
                prefix_one[level][i + 1] = prefix_one[level][i] + ((cur[i] >> bit) & 1);
            zero_count[level] = n - prefix_one[level][n];
            int zi = 0, oi = zero_count[level];
            for (int x : cur) next[((x >> bit) & 1) ? oi++ : zi++] = x;
            cur.swap(next);
        }
    }

    int size() const { return n; }

    T kth_smallest(int l, int r, int k) const {
        assert(0 <= l && l <= r && r <= n && 0 <= k && k < r - l);
        int code = 0;
        for (int level = 0; level < levels; ++level) {
            int ones_l = prefix_one[level][l], ones_r = prefix_one[level][r];
            int zeros = (r - l) - (ones_r - ones_l);
            code <<= 1;
            if (k < zeros) {
                l -= ones_l; r -= ones_r;
            } else {
                code |= 1; k -= zeros;
                l = zero_count[level] + ones_l;
                r = zero_count[level] + ones_r;
            }
        }
        return values[code];
    }

    T kth_largest(int l, int r, int k) const {
        return kth_smallest(l, r, r - l - 1 - k);
    }

    int count_less(int l, int r, const T& upper) const {
        assert(0 <= l && l <= r && r <= n);
        int code = std::lower_bound(values.begin(), values.end(), upper) - values.begin();
        if (code == (int)values.size()) return r - l;
        int answer = 0;
        for (int level = 0; level < levels; ++level) {
            int bit = levels - 1 - level;
            int ones_l = prefix_one[level][l], ones_r = prefix_one[level][r];
            if ((code >> bit) & 1) {
                answer += (r - l) - (ones_r - ones_l);
                l = zero_count[level] + ones_l;
                r = zero_count[level] + ones_r;
            } else {
                l -= ones_l; r -= ones_r;
            }
        }
        return answer;
    }

    int range_freq(int l, int r, const T& lower, const T& upper) const {
        return count_less(l, r, upper) - count_less(l, r, lower);
    }
    int count(int l, int r, const T& value) const {
        auto it = std::lower_bound(values.begin(), values.end(), value);
        if (it == values.end() || *it != value) return 0;
        int code = it - values.begin();
        for (int level = 0; level < levels; ++level) {
            int ones_l = prefix_one[level][l], ones_r = prefix_one[level][r];
            int bit = levels - 1 - level;
            if ((code >> bit) & 1) {
                l = zero_count[level] + ones_l; r = zero_count[level] + ones_r;
            } else {
                l -= ones_l; r -= ones_r;
            }
        }
        return r - l;
    }
    std::optional<T> prev_value(int l, int r, const T& upper) const {
        int count = count_less(l, r, upper);
        if (count == 0) return std::nullopt;
        return kth_smallest(l, r, count - 1);
    }
    std::optional<T> next_value(int l, int r, const T& lower) const {
        int count = count_less(l, r, lower);
        if (count == r - l) return std::nullopt;
        return kth_smallest(l, r, count);
    }

};
