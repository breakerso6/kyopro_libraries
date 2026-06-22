#pragma once
#include <bits/stdc++.h>

template<class T>
struct WeightedDSU {
    std::vector<int> parent, size_;
    std::vector<T> weight;

    explicit WeightedDSU(int n) : parent(n), size_(n, 1), weight(n, T{}) {
        std::iota(parent.begin(), parent.end(), 0);
    }
    int leader(int v) {
        if (parent[v] == v) return v;
        int p = parent[v];
        parent[v] = leader(p);
        weight[v] += weight[p];
        return parent[v];
    }
    T potential(int v) { leader(v); return weight[v]; }
    bool same(int a, int b) { return leader(a) == leader(b); }
    int size(int v) { return size_[leader(v)]; }

    // Adds the constraint potential(b) - potential(a) = w.
    bool merge(int a, int b, T w) {
        w += potential(a) - potential(b);
        a = leader(a); b = leader(b);
        if (a == b) return w == T{};
        if (size_[a] < size_[b]) { std::swap(a, b); w = -w; }
        parent[b] = a;
        weight[b] = w;
        size_[a] += size_[b];
        return true;
    }
    T diff(int a, int b) { return potential(b) - potential(a); }
};
