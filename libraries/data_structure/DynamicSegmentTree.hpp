// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/data_structure/DynamicSegmentTree.hpp
// SPDX-License-Identifier: CC0-1.0

#pragma once
#include <bits/stdc++.h>

template<class S, S (*op)(S, S), S (*e)()>
struct DynamicSegmentTree {
    struct Node { S value; int left = -1, right = -1; };
    long long low, high;
    std::vector<Node> nodes;

    DynamicSegmentTree(long long low_, long long high_) : low(low_), high(high_) {
        assert(low < high);
        nodes.push_back({e(), -1, -1});
    }
    void set(long long position, const S& value) {
        assert(low <= position && position < high);
        set(0, low, high, position, value);
    }
    S get(long long position) const {
        assert(low <= position && position < high);
        int v = 0;
        long long l = low, r = high;
        while (v != -1 && r - l > 1) {
            long long m = l + (r - l) / 2;
            if (position < m) { v = nodes[v].left; r = m; }
            else { v = nodes[v].right; l = m; }
        }
        return node_value(v);
    }
    S prod(long long left, long long right) const {
        assert(low <= left && left <= right && right <= high);
        if (left == right) return e();
        return prod(0, low, high, left, right);
    }

private:
    int make_node() { nodes.push_back({e(), -1, -1}); return (int)nodes.size() - 1; }
    S node_value(int v) const { return v == -1 ? e() : nodes[v].value; }
    void pull(int v) { nodes[v].value = op(node_value(nodes[v].left), node_value(nodes[v].right)); }
    void set(int v, long long l, long long r, long long p, const S& value) {
        if (r - l == 1) { nodes[v].value = value; return; }
        long long m = l + (r - l) / 2;
        bool go_left = p < m;
        int child = go_left ? nodes[v].left : nodes[v].right;
        if (child == -1) {
            child = make_node();
            if (go_left) nodes[v].left = child;
            else nodes[v].right = child;
        }
        if (go_left) set(child, l, m, p, value);
        else set(child, m, r, p, value);
        pull(v);
    }
    S prod(int v, long long l, long long r, long long ql, long long qr) const {
        if (v == -1 || qr <= l || r <= ql) return e();
        if (ql <= l && r <= qr) return nodes[v].value;
        long long m = l + (r - l) / 2;
        if (qr <= m) return prod(nodes[v].left, l, m, ql, qr);
        if (m <= ql) return prod(nodes[v].right, m, r, ql, qr);
        return op(prod(nodes[v].left, l, m, ql, qr), prod(nodes[v].right, m, r, ql, qr));
    }
};
