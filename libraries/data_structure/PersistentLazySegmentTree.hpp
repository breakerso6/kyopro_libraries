#pragma once
#include <bits/stdc++.h>

// Persistent range-add/range-sum segment tree.
template<class T = long long>
struct PersistentLazySegmentTree {
    struct Node { T sum{}, lazy{}; int left = -1, right = -1; };
    int n;
    std::vector<Node> nodes;

    explicit PersistentLazySegmentTree(int n_) : n(n_) { assert(n > 0); }
    int build(const std::vector<T>& values = {}) {
        assert(values.empty() || (int)values.size() == n);
        return build(0, n, values);
    }
    int add(int root, int left, int right, T value) {
        assert(0 <= left && left <= right && right <= n);
        return add(root, left, right, value, 0, n);
    }
    T prod(int root, int left, int right) const {
        assert(0 <= left && left <= right && right <= n);
        return prod(root, left, right, T{}, 0, n);
    }
    T get(int root, int position) const { return prod(root, position, position + 1); }

private:
    int make_node(Node node) { nodes.push_back(node); return (int)nodes.size() - 1; }
    int build(int l, int r, const std::vector<T>& values) {
        if (r - l == 1) return make_node({values.empty() ? T{} : values[l], T{}, -1, -1});
        int m = (l + r) / 2, left = build(l, m, values), right = build(m, r, values);
        return make_node({nodes[left].sum + nodes[right].sum, T{}, left, right});
    }
    int add(int v, int ql, int qr, T value, int l, int r) {
        if (qr <= l || r <= ql) return v;
        Node node = nodes[v];
        if (ql <= l && r <= qr) {
            node.sum += value * T(r - l); node.lazy += value;
            return make_node(node);
        }
        int m = (l + r) / 2;
        node.left = add(node.left, ql, qr, value, l, m);
        node.right = add(node.right, ql, qr, value, m, r);
        node.sum = nodes[node.left].sum + nodes[node.right].sum + node.lazy * T(r - l);
        return make_node(node);
    }
    T prod(int v, int ql, int qr, T inherited, int l, int r) const {
        if (qr <= l || r <= ql) return T{};
        if (ql <= l && r <= qr) return nodes[v].sum + inherited * T(r - l);
        int m = (l + r) / 2; inherited += nodes[v].lazy;
        return prod(nodes[v].left, ql, qr, inherited, l, m)
             + prod(nodes[v].right, ql, qr, inherited, m, r);
    }
};
