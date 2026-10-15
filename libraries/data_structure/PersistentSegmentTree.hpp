// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/data_structure/PersistentSegmentTree.hpp
// SPDX-License-Identifier: CC0-1.0

#pragma once
#include <bits/stdc++.h>

template<class S, S (*op)(S, S), S (*e)()>
struct PersistentSegmentTree {
    struct Node { S value; int left = -1, right = -1; };
    int n;
    std::vector<Node> nodes;

    explicit PersistentSegmentTree(int n_) : n(n_) { assert(n > 0); }
    int build(const std::vector<S>& values = {}) {
        assert(values.empty() || (int)values.size() == n);
        size_t required = nodes.size() + 2 * size_t(n) - 1;
        if (nodes.capacity() < required) nodes.reserve(std::max(required, nodes.capacity() * 2));
        return build(0, n, values);
    }
    int set(int root, int position, const S& value) {
        assert(0 <= position && position < n);
        return set(root, position, value, 0, n);
    }
    S get(int root, int position) const {
        assert(0 <= position && position < n);
        int v = root, l = 0, r = n;
        while (r - l > 1) {
            int m = (l + r) / 2;
            if (position < m) { v = nodes[v].left; r = m; }
            else { v = nodes[v].right; l = m; }
        }
        return nodes[v].value;
    }
    S prod(int root, int left, int right) const {
        assert(0 <= left && left <= right && right <= n);
        if (left == right) return e();
        return prod(root, left, right, 0, n);
    }

private:
    int make_node(S value, int left = -1, int right = -1) {
        nodes.push_back({std::move(value), left, right}); return (int)nodes.size() - 1;
    }
    int build(int l, int r, const std::vector<S>& values) {
        if (r - l == 1) return make_node(values.empty() ? e() : values[l]);
        int m = (l + r) / 2, left = build(l, m, values), right = build(m, r, values);
        return make_node(op(nodes[left].value, nodes[right].value), left, right);
    }
    int set(int v, int position, const S& value, int l, int r) {
        if (r - l == 1) return make_node(value);
        int m = (l + r) / 2, left = nodes[v].left, right = nodes[v].right;
        if (position < m) left = set(left, position, value, l, m);
        else right = set(right, position, value, m, r);
        return make_node(op(nodes[left].value, nodes[right].value), left, right);
    }
    S prod(int v, int ql, int qr, int l, int r) const {
        if (qr <= l || r <= ql) return e();
        if (ql <= l && r <= qr) return nodes[v].value;
        int m = (l + r) / 2;
        return op(prod(nodes[v].left, ql, qr, l, m), prod(nodes[v].right, ql, qr, m, r));
    }
};
