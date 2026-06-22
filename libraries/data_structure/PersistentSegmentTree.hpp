#pragma once
#include <bits/stdc++.h>

template<class S, class Op>
struct PersistentSegmentTree {
    struct Node { S value; int left = -1, right = -1; };
    int n;
    S identity;
    Op op;
    std::vector<Node> nodes;

    PersistentSegmentTree(int n_, S identity_, Op op_ = Op())
        : n(n_), identity(identity_), op(op_) { assert(n > 0); }
    int build(const std::vector<S>& values = {}) {
        assert(values.empty() || (int)values.size() == n);
        return build(0, n, values);
    }
    int set(int root, int position, const S& value) {
        assert(0 <= position && position < n);
        return set(root, position, value, 0, n);
    }
    S get(int root, int position) const { return prod(root, position, position + 1); }
    S prod(int root, int left, int right) const {
        assert(0 <= left && left <= right && right <= n);
        return prod(root, left, right, 0, n);
    }

private:
    int make_node(S value, int left = -1, int right = -1) {
        nodes.push_back({std::move(value), left, right}); return (int)nodes.size() - 1;
    }
    int build(int l, int r, const std::vector<S>& values) {
        if (r - l == 1) return make_node(values.empty() ? identity : values[l]);
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
        if (qr <= l || r <= ql) return identity;
        if (ql <= l && r <= qr) return nodes[v].value;
        int m = (l + r) / 2;
        return op(prod(nodes[v].left, ql, qr, l, m), prod(nodes[v].right, ql, qr, m, r));
    }
};
