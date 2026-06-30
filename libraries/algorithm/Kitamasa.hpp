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
    std::vector<T> result(k), base(k);
    result[0] = 1;
    if (k == 1) base[0] = c[0];
    else base[1] = 1;
    while (n) {
        if (n & 1) result = multiply(result, base);
        base = multiply(base, base);
        n >>= 1;
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
