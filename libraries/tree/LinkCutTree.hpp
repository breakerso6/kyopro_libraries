#pragma once
#include <bits/stdc++.h>

namespace link_cut_tree_detail {
struct NoLazy {};

template<class S>
S default_mapping(NoLazy, S x) {
    return x;
}

inline NoLazy default_composition(NoLazy, NoLazy) {
    return {};
}

inline NoLazy default_id() {
    return {};
}
}

template<class S,
         S (*op)(S, S),
         S (*e)(),
         class F = link_cut_tree_detail::NoLazy,
         S (*mapping)(F, S) = link_cut_tree_detail::default_mapping<S>,
         F (*composition)(F, F) = link_cut_tree_detail::default_composition,
         F (*id_)() = link_cut_tree_detail::default_id>
struct LinkCutTree {
    struct Node {
        S value, prod, rprod;
        F lazy = id_();
        int left = -1, right = -1, parent = -1;
        bool rev = false;
        bool has_lazy = false;
    };

    std::vector<Node> nodes;

    LinkCutTree() = default;
    explicit LinkCutTree(int n) { build(std::vector<S>(n, e())); }
    explicit LinkCutTree(const std::vector<S>& values) { build(values); }

    void build(const std::vector<S>& values) {
        nodes.clear();
        nodes.reserve(values.size());
        for (const S& value : values) nodes.push_back({value, value, value, id_(), -1, -1, -1, false, false});
    }

    int size() const { return (int)nodes.size(); }

    void set(int v, const S& value) {
        assert(0 <= v && v < size());
        access(v);
        nodes[v].value = value;
        pull(v);
    }

    S get(int v) {
        assert(0 <= v && v < size());
        access(v);
        return nodes[v].value;
    }

    bool connected(int u, int v) {
        assert(0 <= u && u < size());
        assert(0 <= v && v < size());
        if (u == v) return true;
        return root(u) == root(v);
    }

    bool link(int u, int v) {
        assert(0 <= u && u < size());
        assert(0 <= v && v < size());
        if (connected(u, v)) return false;
        evert(u);
        nodes[u].parent = v;
        return true;
    }

    bool cut(int u, int v) {
        assert(0 <= u && u < size());
        assert(0 <= v && v < size());
        evert(u);
        access(v);
        if (nodes[v].left != u || nodes[u].right != -1) return false;
        nodes[v].left = -1;
        nodes[u].parent = -1;
        pull(v);
        return true;
    }

    void evert(int v) {
        assert(0 <= v && v < size());
        access(v);
        apply_reverse(v);
    }

    int root(int v) {
        assert(0 <= v && v < size());
        access(v);
        while (true) {
            push(v);
            if (nodes[v].left == -1) break;
            v = nodes[v].left;
        }
        splay(v);
        return v;
    }

    int lca(int u, int v) {
        assert(0 <= u && u < size());
        assert(0 <= v && v < size());
        if (!connected(u, v)) return -1;
        access(u);
        return access(v);
    }

    S prod(int u, int v) {
        assert(0 <= u && u < size());
        assert(0 <= v && v < size());
        assert(connected(u, v));
        evert(u);
        access(v);
        return nodes[v].prod;
    }

    void apply(int u, int v, F f) {
        assert(0 <= u && u < size());
        assert(0 <= v && v < size());
        assert(connected(u, v));
        evert(u);
        access(v);
        apply_lazy(v, f);
    }

private:
    S node_prod(int v) const { return v == -1 ? e() : nodes[v].prod; }
    S node_rprod(int v) const { return v == -1 ? e() : nodes[v].rprod; }

    bool is_root(int v) const {
        int p = nodes[v].parent;
        return p == -1 || (nodes[p].left != v && nodes[p].right != v);
    }

    void pull(int v) {
        if (v == -1) return;
        nodes[v].prod = op(op(node_prod(nodes[v].left), nodes[v].value), node_prod(nodes[v].right));
        nodes[v].rprod = op(op(node_rprod(nodes[v].right), nodes[v].value), node_rprod(nodes[v].left));
    }

    void apply_reverse(int v) {
        if (v == -1) return;
        std::swap(nodes[v].left, nodes[v].right);
        std::swap(nodes[v].prod, nodes[v].rprod);
        nodes[v].rev = !nodes[v].rev;
    }

    void apply_lazy(int v, F f) {
        if (v == -1) return;
        nodes[v].value = mapping(f, nodes[v].value);
        nodes[v].prod = mapping(f, nodes[v].prod);
        nodes[v].rprod = mapping(f, nodes[v].rprod);
        nodes[v].lazy = nodes[v].has_lazy ? composition(f, nodes[v].lazy) : f;
        nodes[v].has_lazy = true;
    }

    void push(int v) {
        if (v == -1) return;
        if (nodes[v].rev) {
            apply_reverse(nodes[v].left);
            apply_reverse(nodes[v].right);
            nodes[v].rev = false;
        }
        if (nodes[v].has_lazy) {
            apply_lazy(nodes[v].left, nodes[v].lazy);
            apply_lazy(nodes[v].right, nodes[v].lazy);
            nodes[v].lazy = id_();
            nodes[v].has_lazy = false;
        }
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
        if (g != -1) {
            if (nodes[g].left == p) nodes[g].left = v;
            if (nodes[g].right == p) nodes[g].right = v;
        }
        pull(p);
        pull(v);
    }

    void splay(int v) {
        std::vector<int> path;
        for (int u = v; u != -1; u = nodes[u].parent) {
            path.push_back(u);
            if (is_root(u)) break;
        }
        for (int i = (int)path.size() - 1; i >= 0; --i) push(path[i]);
        while (!is_root(v)) {
            int p = nodes[v].parent;
            int g = nodes[p].parent;
            if (!is_root(p)) {
                if ((nodes[p].left == v) == (nodes[g].left == p)) rotate(p);
                else rotate(v);
            }
            rotate(v);
        }
    }

    int access(int v) {
        int last = -1;
        for (int u = v; u != -1; u = nodes[u].parent) {
            splay(u);
            nodes[u].right = last;
            if (last != -1) nodes[last].parent = u;
            pull(u);
            last = u;
        }
        splay(v);
        return last;
    }
};
