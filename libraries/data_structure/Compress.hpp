#pragma once
#include <bits/stdc++.h>

template<class T>
struct Compress {
    std::vector<T> values;

    Compress() = default;
    explicit Compress(const std::vector<T>& xs) { build(xs); }

    void add(const T& x) {
        values.push_back(x);
    }
    void add(const std::vector<T>& xs) {
        values.insert(values.end(), xs.begin(), xs.end());
    }
    void build() {
        std::sort(values.begin(), values.end());
        values.erase(std::unique(values.begin(), values.end()), values.end());
    }
    void build(const std::vector<T>& xs) {
        values = xs;
        build();
    }

    int size() const {
        return (int)values.size();
    }
    bool empty() const {
        return values.empty();
    }
    const T& operator[](int i) const {
        assert(0 <= i && i < size());
        return values[i];
    }

    int lower_bound(const T& x) const {
        return std::lower_bound(values.begin(), values.end(), x) - values.begin();
    }
    int upper_bound(const T& x) const {
        return std::upper_bound(values.begin(), values.end(), x) - values.begin();
    }
    bool contains(const T& x) const {
        int i = lower_bound(x);
        return i < size() && values[i] == x;
    }
    int index(const T& x) const {
        int i = lower_bound(x);
        assert(i < size() && values[i] == x);
        return i;
    }
    int operator()(const T& x) const {
        return index(x);
    }

    std::vector<int> compressed(const std::vector<T>& xs) const {
        std::vector<int> result;
        result.reserve(xs.size());
        for (const T& x : xs) result.push_back(index(x));
        return result;
    }
};
