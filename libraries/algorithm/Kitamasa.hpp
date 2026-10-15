// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/algorithm/Kitamasa.hpp
// SPDX-License-Identifier: CC0-1.0

#pragma once
#include <bits/stdc++.h>

template<class T>
std::vector<T> kitamasa_coefficients(long long n, const std::vector<T>& c) {
    int k = (int)c.size();
    assert(k > 0 && n >= 0);
    auto multiply = [&](const std::vector<T>& a, const std::vector<T>& b) {
        std::vector<T> tmp(2 * k - 1);
        for (int i = 0; i < k; ++i) for (int j = 0; j < k; ++j) tmp[i + j] += a[i] * b[j];
        for (int i = 2 * k - 2; i >= k; --i) for (int j = 1; j <= k; ++j) tmp[i - j] += tmp[i] * c[k - j];
        tmp.resize(k);
        return tmp;
    };
    std::vector<T> result(k);
    result[0] = 1;
    if (n < k) {
        result[0] = 0;
        result[(int)n] = 1;
        return result;
    }
    // Read bits from the most significant end: multiply by x in O(k)
    // instead of multiplying two dense polynomials for each set bit.
    int highest = 0;
    for (long long bits = n; bits > 1; bits >>= 1) ++highest;
    for (int bit = highest; bit >= 0; --bit) {
        if (bit != highest) result = multiply(result, result);
        if ((n >> bit) & 1) {
            T last = result.back();
            for (int i = k - 1; i > 0; --i) result[i] = result[i - 1] + last * c[i];
            result[0] = last * c[0];
        }
    }
    return result;
}

template<class T>
T kitamasa(long long n, const std::vector<T>& initial, const std::vector<T>& c) {
    assert(initial.size() == c.size());
    auto coef = kitamasa_coefficients<T>(n, c);
    T answer = 0;
    for (int i = 0; i < (int)coef.size(); ++i) answer += coef[i] * initial[i];
    return answer;
}
