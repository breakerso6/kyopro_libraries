#pragma once
#include <bits/stdc++.h>

struct Manacher {
    std::vector<int> odd, even;

    Manacher() = default;
    explicit Manacher(const std::string& s) { build(s); }
    void build(const std::string& s) {
        int n = (int)s.size(); odd.assign(n, 0); even.assign(n, 0);
        for (int i = 0, l = 0, r = -1; i < n; ++i) {
            int k = i > r ? 1 : std::min(odd[l + r - i], r - i + 1);
            while (i - k >= 0 && i + k < n && s[i - k] == s[i + k]) ++k;
            odd[i] = k--;
            if (i + k > r) l = i - k, r = i + k;
        }
        for (int i = 0, l = 0, r = -1; i < n; ++i) {
            int k = i > r ? 0 : std::min(even[l + r - i + 1], r - i + 1);
            while (i - k - 1 >= 0 && i + k < n && s[i - k - 1] == s[i + k]) ++k;
            even[i] = k--;
            if (i + k > r) l = i - k - 1, r = i + k;
        }
    }
    bool is_palindrome(int l, int r) const {
        assert(0 <= l && l <= r && r <= (int)odd.size());
        int length = r - l;
        if (length == 0) return true;
        if (length & 1) return odd[(l + r) / 2] >= length / 2 + 1;
        return even[(l + r) / 2] >= length / 2;
    }
};
