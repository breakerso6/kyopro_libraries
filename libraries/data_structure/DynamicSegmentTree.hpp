#pragma once
#include <bits/stdc++.h>

template<class S, class Op>
struct DynamicSegmentTree {
    struct Node { S value; int left = -1, right = -1; };
    long long low, high;
    S identity;
    Op op;
    std::vector<Node> nodes;

    DynamicSegmentTree(long long low_, long long high_, S identity_, Op op_ = Op())
        : low(low_), high(high_), identity(identity_), op(op_) {
        assert(low < high);
        nodes.push_back({identity, -1, -1});
    }
    void set(long long position, const S& value) {
        assert(low <= position && position < high);
        set(0, low, high, position, value);
    }
    S get(long long position) const { return prod(position, position + 1); }
    S prod(long long left, long long right) const {
        assert(low <= left && left <= right && right <= high);
        return prod(0, low, high, left, right);
    }

private:
    int make_node() { nodes.push_back({identity, -1, -1}); return (int)nodes.size() - 1; }
    S node_value(int v) const { return v == -1 ? identity : nodes[v].value; }
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
        if (v == -1 || qr <= l || r <= ql) return identity;
        if (ql <= l && r <= qr) return nodes[v].value;
        long long m = l + (r - l) / 2;
        return op(prod(nodes[v].left, l, m, ql, qr), prod(nodes[v].right, m, r, ql, qr));
    }
};
