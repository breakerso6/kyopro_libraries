#pragma once
#include <bits/stdc++.h>

template<class S, S (*op)(S, S), S (*e)()>
struct SplayTreeSequence {
    struct Node {
        S value, prod, rprod;
        int left = -1, right = -1, parent = -1, size = 1;
        bool rev = false;
    };

    int root = -1;
    std::vector<Node> nodes;

    SplayTreeSequence() = default;
    explicit SplayTreeSequence(const std::vector<S>& values) { build(values); }

    int size() const { return node_size(root); }
    bool empty() const { return root == -1; }

    void build(const std::vector<S>& values) {
        root = -1;
        nodes.clear();
        nodes.reserve(values.size());
        for (const S& value : values) root = merge(root, make_node(value));
    }

    void insert(int position, const S& value) {
        assert(0 <= position && position <= size());
        auto [a, b] = split(root, position);
        root = merge(merge(a, make_node(value)), b);
    }

    void erase(int position) {
        assert(0 <= position && position < size());
        auto [a, bc] = split(root, position);
        auto [b, c] = split(bc, 1);
        (void)b;
        root = merge(a, c);
    }

    void set(int position, const S& value) {
        assert(0 <= position && position < size());
        root = kth(root, position);
        nodes[root].value = value;
        pull(root);
    }

    S get(int position) {
        assert(0 <= position && position < size());
        root = kth(root, position);
        return nodes[root].value;
    }

    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return e();
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        S result = node_prod(b);
        root = merge(a, merge(b, c));
        return result;
    }

    void reverse(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        apply_reverse(b);
        root = merge(a, merge(b, c));
    }

    std::vector<S> to_vector() {
        std::vector<S> result;
        result.reserve(size());
        dfs(root, result);
        return result;
    }

private:
    int node_size(int v) const { return v == -1 ? 0 : nodes[v].size; }
    S node_prod(int v) const { return v == -1 ? e() : nodes[v].prod; }
    S node_rprod(int v) const { return v == -1 ? e() : nodes[v].rprod; }

    int make_node(const S& value) {
        nodes.push_back({value, value, value, -1, -1, -1, 1, false});
        return (int)nodes.size() - 1;
    }

    void pull(int v) {
        if (v == -1) return;
        nodes[v].size = 1 + node_size(nodes[v].left) + node_size(nodes[v].right);
        nodes[v].prod = op(op(node_prod(nodes[v].left), nodes[v].value), node_prod(nodes[v].right));
        nodes[v].rprod = op(op(node_rprod(nodes[v].right), nodes[v].value), node_rprod(nodes[v].left));
    }

    void apply_reverse(int v) {
        if (v == -1) return;
        std::swap(nodes[v].left, nodes[v].right);
        std::swap(nodes[v].prod, nodes[v].rprod);
        nodes[v].rev = !nodes[v].rev;
    }

    void push(int v) {
        if (v == -1 || !nodes[v].rev) return;
        apply_reverse(nodes[v].left);
        apply_reverse(nodes[v].right);
        nodes[v].rev = false;
    }

    void rotate(int v) {
        int p = nodes[v].parent;
        int g = nodes[p].parent;
        push(p);
        push(v);
        if (nodes[p].left == v) {
            int b = nodes[v].right;
            nodes[v].right = p;
            nodes[p].left = b;
            if (b != -1) nodes[b].parent = p;
        } else {
            int b = nodes[v].left;
            nodes[v].left = p;
            nodes[p].right = b;
            if (b != -1) nodes[b].parent = p;
        }
        nodes[p].parent = v;
        nodes[v].parent = g;
        if (g == -1) root = v;
        else if (nodes[g].left == p) nodes[g].left = v;
        else nodes[g].right = v;
        pull(p);
        pull(v);
    }

    void splay(int v) {
        std::vector<int> path;
        for (int u = v; u != -1; u = nodes[u].parent) path.push_back(u);
        for (int i = (int)path.size() - 1; i >= 0; --i) push(path[i]);
        while (nodes[v].parent != -1) {
            int p = nodes[v].parent;
            int g = nodes[p].parent;
            if (g == -1) {
                rotate(v);
            } else if ((nodes[g].left == p) == (nodes[p].left == v)) {
                rotate(p);
                rotate(v);
            } else {
                rotate(v);
                rotate(v);
            }
        }
        root = v;
    }

    int kth(int v, int k) {
        root = v;
        nodes[root].parent = -1;
        while (true) {
            push(v);
            int left_size = node_size(nodes[v].left);
            if (k < left_size) {
                v = nodes[v].left;
            } else if (k == left_size) {
                splay(v);
                return root;
            } else {
                k -= left_size + 1;
                v = nodes[v].right;
            }
        }
    }

    std::pair<int, int> split(int v, int k) {
        if (v == -1) return {-1, -1};
        nodes[v].parent = -1;
        if (k == 0) return {-1, v};
        if (k == node_size(v)) return {v, -1};
        int b = kth(v, k);
        int a = nodes[b].left;
        nodes[b].left = -1;
        if (a != -1) nodes[a].parent = -1;
        pull(b);
        return {a, b};
    }

    int merge(int a, int b) {
        if (a == -1) {
            if (b != -1) nodes[b].parent = -1;
            return b;
        }
        if (b == -1) {
            nodes[a].parent = -1;
            return a;
        }
        a = kth(a, node_size(a) - 1);
        nodes[a].right = b;
        nodes[b].parent = a;
        pull(a);
        return a;
    }

    void dfs(int v, std::vector<S>& result) {
        if (v == -1) return;
        push(v);
        dfs(nodes[v].left, result);
        result.push_back(nodes[v].value);
        dfs(nodes[v].right, result);
    }
};
