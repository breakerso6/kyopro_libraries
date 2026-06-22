#pragma once
#include <bits/stdc++.h>

template<class S, class Op>
struct SparseTable {
    int n = 0;
    Op op;
    std::vector<int> lg;
    std::vector<std::vector<S>> table;

    SparseTable() = default;
    SparseTable(const std::vector<S>& a, Op op_ = Op()) : n((int)a.size()), op(op_) {
        lg.assign(n + 1, 0);
        for (int i = 2; i <= n; ++i) lg[i] = lg[i / 2] + 1;
        if (!n) return;
        table.assign(lg[n] + 1, a);
        for (int k = 1; k <= lg[n]; ++k)
            for (int i = 0; i + (1 << k) <= n; ++i)
                table[k][i] = op(table[k - 1][i], table[k - 1][i + (1 << (k - 1))]);
    }
    // op must be idempotent. Queries [l, r).
    S prod(int l, int r) const {
        assert(0 <= l && l < r && r <= n);
        int k = lg[r - l];
        return op(table[k][l], table[k][r - (1 << k)]);
    }
};
