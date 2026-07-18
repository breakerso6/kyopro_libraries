#pragma once
#include <bits/stdc++.h>

namespace implicit_treap_detail {
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
         class F = implicit_treap_detail::NoLazy,
         S (*mapping)(F, S) = implicit_treap_detail::default_mapping<S>,
         F (*composition)(F, F) = implicit_treap_detail::default_composition,
         F (*id_)() = implicit_treap_detail::default_id>
struct ImplicitTreap {
    using Handle = int;

    struct Node {
        S value, prod, rprod;
        F lazy = id_();
        int left = -1, right = -1, parent = -1, size = 1;
        uint32_t priority = 0;
        bool rev = false;
        bool has_lazy = false;
        bool active = true;
    };

    int root = -1;
    std::vector<Node> nodes;
    uint64_t seed = 88172645463325252ULL;

    ImplicitTreap() = default;
    explicit ImplicitTreap(const std::vector<S>& values) { build(values); }

    int size() const { return node_size(root); }
    bool empty() const { return root == -1; }

    void build(const std::vector<S>& values) {
        root = -1;
        nodes.clear();
        nodes.reserve(values.size());
        std::vector<int> stack;
        stack.reserve(values.size());
        for (const S& value : values) {
            int v = make_node(value);
            int last = -1;
            while (!stack.empty() && nodes[v].priority < nodes[stack.back()].priority) {
                last = stack.back();
                stack.pop_back();
            }
            if (!stack.empty()) nodes[stack.back()].right = v;
            nodes[v].left = last;
            stack.push_back(v);
        }
        root = stack.empty() ? -1 : stack.front();
        rebuild(root);
    }

    Handle insert(int position, const S& value) {
        assert(0 <= position && position <= size());
        Handle handle = make_node(value);
        auto [a, b] = split(root, position);
        root = merge(merge(a, handle), b);
        return handle;
    }

    void erase(int position) {
        (void)extract(position);
    }

    Handle extract(int position) {
        assert(0 <= position && position < size());
        auto [a, bc] = split(root, position);
        auto [b, c] = split(bc, 1);
        root = merge(a, c);
        make_singleton(b);
        nodes[b].active = false;
        return b;
    }

    void reinsert(int position, Handle handle) {
        assert(0 <= position && position <= size());
        assert(valid_handle(handle) && !nodes[handle].active);
        make_singleton(handle);
        nodes[handle].active = true;
        auto [a, b] = split(root, position);
        root = merge(merge(a, handle), b);
    }

    void reinsert(int position, Handle handle, const S& value) {
        assert(valid_handle(handle) && !nodes[handle].active);
        nodes[handle].value = value;
        nodes[handle].lazy = id_();
        nodes[handle].has_lazy = false;
        nodes[handle].rev = false;
        make_singleton(handle);
        reinsert(position, handle);
    }

    Handle handle_at(int position) {
        assert(0 <= position && position < size());
        return kth(root, position);
    }

    int index_of(Handle handle) {
        assert(valid_handle(handle) && nodes[handle].active);
        push_path(handle);
        int result = node_size(nodes[handle].left);
        for (int v = handle; nodes[v].parent != -1; ) {
            int p = nodes[v].parent;
            if (nodes[p].right == v) {
                result += node_size(nodes[p].left) + 1;
            }
            v = p;
        }
        return result;
    }

    void set(int position, const S& value) {
        assert(0 <= position && position < size());
        auto [a, bc] = split(root, position);
        auto [b, c] = split(bc, 1);
        nodes[b].value = value;
        nodes[b].lazy = id_();
        nodes[b].has_lazy = false;
        pull(b);
        root = merge(a, merge(b, c));
    }

    S get(int position) {
        assert(0 <= position && position < size());
        return nodes[kth(root, position)].value;
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

    void apply(int l, int r, F f) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        apply_lazy(b, f);
        root = merge(a, merge(b, c));
    }

    std::vector<S> to_vector() {
        std::vector<S> result;
        result.reserve(size());
        dfs(root, result);
        return result;
    }

    std::vector<Handle> to_handles() {
        std::vector<Handle> result;
        result.reserve(size());
        dfs_handles(root, result);
        return result;
    }

private:
    int node_size(int v) const { return v == -1 ? 0 : nodes[v].size; }
    S node_prod(int v) const { return v == -1 ? e() : nodes[v].prod; }
    S node_rprod(int v) const { return v == -1 ? e() : nodes[v].rprod; }

    uint32_t rng() {
        seed ^= seed << 7;
        seed ^= seed >> 9;
        return (uint32_t)seed;
    }

    int make_node(const S& value) {
        nodes.push_back({value, value, value, id_(), -1, -1, -1, 1, rng(), false, false, true});
        return (int)nodes.size() - 1;
    }

    bool valid_handle(Handle handle) const {
        return 0 <= handle && handle < (int)nodes.size();
    }

    void make_singleton(int v) {
        nodes[v].left = -1;
        nodes[v].right = -1;
        nodes[v].parent = -1;
        nodes[v].size = 1;
        nodes[v].prod = nodes[v].value;
        nodes[v].rprod = nodes[v].value;
        nodes[v].lazy = id_();
        nodes[v].rev = false;
        nodes[v].has_lazy = false;
    }

    void pull(int v) {
        if (v == -1) return;
        nodes[v].size = 1 + node_size(nodes[v].left) + node_size(nodes[v].right);
        nodes[v].prod = op(op(node_prod(nodes[v].left), nodes[v].value), node_prod(nodes[v].right));
        nodes[v].rprod = op(op(node_rprod(nodes[v].right), nodes[v].value), node_rprod(nodes[v].left));
    }

    void rebuild(int v) {
        if (v == -1) return;
        nodes[v].parent = -1;
        std::vector<std::pair<int, bool>> stack{{v, false}};
        while (!stack.empty()) {
            auto [u, visited] = stack.back();
            stack.pop_back();
            if (u == -1) continue;
            if (visited) {
                pull(u);
            } else {
                if (nodes[u].left != -1) nodes[nodes[u].left].parent = u;
                if (nodes[u].right != -1) nodes[nodes[u].right].parent = u;
                stack.push_back({u, true});
                stack.push_back({nodes[u].right, false});
                stack.push_back({nodes[u].left, false});
            }
        }
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

    void push_path(int v) {
        std::vector<int> path;
        for (int u = v; u != -1; u = nodes[u].parent) path.push_back(u);
        std::reverse(path.begin(), path.end());
        for (int u : path) push(u);
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
        if (nodes[a].priority < nodes[b].priority) {
            push(a);
            nodes[a].right = merge(nodes[a].right, b);
            nodes[nodes[a].right].parent = a;
            nodes[a].parent = -1;
            pull(a);
            return a;
        } else {
            push(b);
            nodes[b].left = merge(a, nodes[b].left);
            nodes[nodes[b].left].parent = b;
            nodes[b].parent = -1;
            pull(b);
            return b;
        }
    }

    std::pair<int, int> split(int v, int k) {
        if (v == -1) return {-1, -1};
        push(v);
        if (node_size(nodes[v].left) >= k) {
            auto [a, b] = split(nodes[v].left, k);
            nodes[v].left = b;
            if (b != -1) nodes[b].parent = v;
            if (a != -1) nodes[a].parent = -1;
            nodes[v].parent = -1;
            pull(v);
            return {a, v};
        } else {
            auto [a, b] = split(nodes[v].right, k - node_size(nodes[v].left) - 1);
            nodes[v].right = a;
            if (a != -1) nodes[a].parent = v;
            if (b != -1) nodes[b].parent = -1;
            nodes[v].parent = -1;
            pull(v);
            return {v, b};
        }
    }

    int kth(int v, int k) {
        push(v);
        int left_size = node_size(nodes[v].left);
        if (k < left_size) return kth(nodes[v].left, k);
        if (k == left_size) return v;
        return kth(nodes[v].right, k - left_size - 1);
    }

    void dfs(int v, std::vector<S>& result) {
        if (v == -1) return;
        push(v);
        dfs(nodes[v].left, result);
        result.push_back(nodes[v].value);
        dfs(nodes[v].right, result);
    }

    void dfs_handles(int v, std::vector<Handle>& result) {
        if (v == -1) return;
        push(v);
        dfs_handles(nodes[v].left, result);
        result.push_back(v);
        dfs_handles(nodes[v].right, result);
    }
};
