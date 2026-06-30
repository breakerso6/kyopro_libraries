#pragma once
#include <bits/stdc++.h>

template<class Container>
std::vector<int> z_algorithm(const Container& s) {
    int n = (int)s.size();
    std::vector<int> z(n);
    if (n == 0) return z;
    z[0] = n;
    for (int i = 1, j = 0; i < n; ++i) {
        int& k = z[i];
        if (i < j + z[j]) k = std::min(j + z[j] - i, z[i - j]);
        while (i + k < n && s[k] == s[i + k]) ++k;
        if (j + z[j] < i + z[i]) j = i;
    }
    return z;
}
