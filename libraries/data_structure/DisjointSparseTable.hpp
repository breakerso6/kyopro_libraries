#pragma once
#include <bits/stdc++.h>

template<class S, class Op>
struct DisjointSparseTable {
    int n = 0;
    Op op;
    std::vector<S> base;
    std::vector<std::vector<S>> table;

    DisjointSparseTable() = default;
    DisjointSparseTable(const std::vector<S>& a, Op op_ = Op()) : n((int)a.size()), op(op_), base(a) {
        int levels = 0;
        while ((1 << levels) < std::max(1, n)) ++levels;
        table.assign(levels, a);
        for (int k = 0; k < levels; ++k) {
            int len = 1 << (k + 1), half = len >> 1;
            for (int block = 0; block < n; block += len) {
                int mid = std::min(block + half, n), end = std::min(block + len, n);
                if (mid == block) continue;
                for (int i = mid - 2; i >= block; --i) table[k][i] = op(a[i], table[k][i + 1]);
                if (mid < end)
                    for (int i = mid + 1; i < end; ++i) table[k][i] = op(table[k][i - 1], a[i]);
            }
        }
    }
    // Associativity is sufficient. Queries [l, r).
    S prod(int l, int r) const {
        assert(0 <= l && l < r && r <= n);
        if (r - l == 1) return base[l];
        int k = 31 - __builtin_clz(l ^ (r - 1));
        return op(table[k][l], table[k][r - 1]);
    }
};
