#pragma once
#include <bits/stdc++.h>

// Fully persistent union-find. Each operation receives and returns a root id.
struct PersistentDSU {
    struct Node { int value = -1, left = -1, right = -1; };
    int n;
    std::vector<Node> nodes;

    explicit PersistentDSU(int n_) : n(n_) { assert(n > 0); }
    int build() { return build(0, n); }
    int leader(int root, int v) const {
        assert(0 <= v && v < n);
        int parent;
        while ((parent = get(root, v, 0, n)) >= 0) v = parent;
        return v;
    }
    bool same(int root, int a, int b) const { return leader(root, a) == leader(root, b); }
    int size(int root, int v) const { return -get(root, leader(root, v), 0, n); }
    int merge(int root, int a, int b) {
        a = leader(root, a); b = leader(root, b);
        if (a == b) return root;
        int as = -get(root, a, 0, n), bs = -get(root, b, 0, n);
        if (as < bs) { std::swap(a, b); std::swap(as, bs); }
        int next = set(root, a, -(as + bs), 0, n);
        return set(next, b, a, 0, n);
    }

private:
    int make_node(Node node) { nodes.push_back(node); return (int)nodes.size() - 1; }
    int build(int l, int r) {
        if (r - l == 1) return make_node({-1, -1, -1});
        int m = (l + r) / 2, left = build(l, m), right = build(m, r);
        return make_node({-1, left, right});
    }
    int get(int v, int position, int l, int r) const {
        if (r - l == 1) return nodes[v].value;
        int m = (l + r) / 2;
        return position < m ? get(nodes[v].left, position, l, m) : get(nodes[v].right, position, m, r);
    }
    int set(int v, int position, int value, int l, int r) {
        Node node = nodes[v];
        if (r - l == 1) { node.value = value; return make_node(node); }
        int m = (l + r) / 2;
        if (position < m) node.left = set(node.left, position, value, l, m);
        else node.right = set(node.right, position, value, m, r);
        return make_node(node);
    }
};
