#pragma once
#include <bits/stdc++.h>

struct ImplicitTreapBeats {
    using i64 = long long;
    static constexpr i64 INF = std::numeric_limits<i64>::max() / 4;

    struct Node {
        i64 value = 0;
        i64 sum = 0, add = 0;
        i64 max1 = -INF, max2 = -INF, min1 = INF, min2 = INF;
        int max_count = 0, min_count = 0;
        int left = -1, right = -1, size = 1;
        uint32_t priority = 0;
        bool rev = false;
    };

    int root = -1;
    std::vector<Node> nodes;
    uint64_t seed = 88172645463325252ULL;

    ImplicitTreapBeats() = default;
    explicit ImplicitTreapBeats(const std::vector<i64>& values) { build(values); }

    int size() const { return node_size(root); }
    bool empty() const { return root == -1; }

    void build(const std::vector<i64>& values) {
        root = -1;
        nodes.clear();
        nodes.reserve(values.size());
        for (i64 value : values) root = merge(root, make_node(value));
    }

    void insert(int position, i64 value) {
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

    void set(int position, i64 value) {
        assert(0 <= position && position < size());
        auto [a, bc] = split(root, position);
        auto [b, c] = split(bc, 1);
        reset_node(b, value);
        root = merge(a, merge(b, c));
    }

    i64 get(int position) {
        assert(0 <= position && position < size());
        return nodes[kth(root, position)].value;
    }

    void reverse(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        apply_reverse(b);
        root = merge(a, merge(b, c));
    }

    void range_chmin(int l, int r, i64 x) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        subtree_chmin(b, x);
        root = merge(a, merge(b, c));
    }

    void range_chmax(int l, int r, i64 x) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        subtree_chmax(b, x);
        root = merge(a, merge(b, c));
    }

    void range_add(int l, int r, i64 x) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        apply_add(b, x);
        root = merge(a, merge(b, c));
    }

    i64 range_sum(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return 0;
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        i64 result = node_sum(b);
        root = merge(a, merge(b, c));
        return result;
    }

    i64 range_min(int l, int r) {
        assert(0 <= l && l < r && r <= size());
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        i64 result = nodes[b].min1;
        root = merge(a, merge(b, c));
        return result;
    }

    i64 range_max(int l, int r) {
        assert(0 <= l && l < r && r <= size());
        auto [a, bc] = split(root, l);
        auto [b, c] = split(bc, r - l);
        i64 result = nodes[b].max1;
        root = merge(a, merge(b, c));
        return result;
    }

    std::vector<i64> to_vector() {
        std::vector<i64> result;
        result.reserve(size());
        dfs(root, result);
        return result;
    }

private:
    int node_size(int v) const { return v == -1 ? 0 : nodes[v].size; }
    i64 node_sum(int v) const { return v == -1 ? 0 : nodes[v].sum; }

    uint32_t rng() {
        seed ^= seed << 7;
        seed ^= seed >> 9;
        return (uint32_t)seed;
    }

    int make_node(i64 value) {
        nodes.push_back(Node());
        int v = (int)nodes.size() - 1;
        nodes[v].priority = rng();
        reset_node(v, value);
        return v;
    }

    void reset_node(int v, i64 value) {
        int left = nodes[v].left, right = nodes[v].right;
        uint32_t priority = nodes[v].priority;
        nodes[v] = Node();
        nodes[v].left = left;
        nodes[v].right = right;
        nodes[v].priority = priority;
        nodes[v].value = value;
        pull(v);
    }

    static Node singleton(i64 value) {
        Node x;
        x.value = value;
        x.sum = value;
        x.max1 = x.min1 = value;
        x.max2 = -INF;
        x.min2 = INF;
        x.max_count = x.min_count = 1;
        x.size = 1;
        return x;
    }

    static Node merge_info(const Node& a, const Node& b) {
        Node x;
        x.size = a.size + b.size;
        x.sum = a.sum + b.sum;
        if (a.max1 > b.max1) {
            x.max1 = a.max1;
            x.max_count = a.max_count;
            x.max2 = std::max(a.max2, b.max1);
        } else if (a.max1 < b.max1) {
            x.max1 = b.max1;
            x.max_count = b.max_count;
            x.max2 = std::max(a.max1, b.max2);
        } else {
            x.max1 = a.max1;
            x.max_count = a.max_count + b.max_count;
            x.max2 = std::max(a.max2, b.max2);
        }
        if (a.min1 < b.min1) {
            x.min1 = a.min1;
            x.min_count = a.min_count;
            x.min2 = std::min(a.min2, b.min1);
        } else if (a.min1 > b.min1) {
            x.min1 = b.min1;
            x.min_count = b.min_count;
            x.min2 = std::min(a.min1, b.min2);
        } else {
            x.min1 = a.min1;
            x.min_count = a.min_count + b.min_count;
            x.min2 = std::min(a.min2, b.min2);
        }
        return x;
    }

    void pull(int v) {
        if (v == -1) return;
        Node info = singleton(nodes[v].value);
        if (nodes[v].left != -1) info = merge_info(nodes[nodes[v].left], info);
        if (nodes[v].right != -1) info = merge_info(info, nodes[nodes[v].right]);
        i64 value = nodes[v].value;
        i64 add = nodes[v].add;
        int left = nodes[v].left, right = nodes[v].right;
        uint32_t priority = nodes[v].priority;
        bool rev = nodes[v].rev;
        nodes[v] = info;
        nodes[v].value = value;
        nodes[v].add = add;
        nodes[v].left = left;
        nodes[v].right = right;
        nodes[v].priority = priority;
        nodes[v].rev = rev;
    }

    void apply_reverse(int v) {
        if (v == -1) return;
        std::swap(nodes[v].left, nodes[v].right);
        nodes[v].rev = !nodes[v].rev;
    }

    void apply_add(int v, i64 x) {
        if (v == -1) return;
        Node& a = nodes[v];
        a.value += x;
        a.sum += x * a.size;
        a.max1 += x;
        if (a.max2 != -INF) a.max2 += x;
        a.min1 += x;
        if (a.min2 != INF) a.min2 += x;
        a.add += x;
    }

    void apply_chmin(int v, i64 x) {
        if (v == -1 || nodes[v].max1 <= x) return;
        Node& a = nodes[v];
        a.sum += (x - a.max1) * a.max_count;
        if (a.value > x) a.value = x;
        if (a.min1 == a.max1) a.min1 = x;
        else if (a.min2 == a.max1) a.min2 = x;
        a.max1 = x;
    }

    void apply_chmax(int v, i64 x) {
        if (v == -1 || nodes[v].min1 >= x) return;
        Node& a = nodes[v];
        a.sum += (x - a.min1) * a.min_count;
        if (a.value < x) a.value = x;
        if (a.max1 == a.min1) a.max1 = x;
        else if (a.max2 == a.min1) a.max2 = x;
        a.min1 = x;
    }

    void push(int v) {
        if (v == -1) return;
        if (nodes[v].rev) {
            apply_reverse(nodes[v].left);
            apply_reverse(nodes[v].right);
            nodes[v].rev = false;
        }
        if (nodes[v].add != 0) {
            apply_add(nodes[v].left, nodes[v].add);
            apply_add(nodes[v].right, nodes[v].add);
            nodes[v].add = 0;
        }
        apply_chmin(nodes[v].left, nodes[v].max1);
        apply_chmin(nodes[v].right, nodes[v].max1);
        apply_chmax(nodes[v].left, nodes[v].min1);
        apply_chmax(nodes[v].right, nodes[v].min1);
    }

    void subtree_chmin(int v, i64 x) {
        if (v == -1 || nodes[v].max1 <= x) return;
        if (nodes[v].max2 < x) {
            apply_chmin(v, x);
            return;
        }
        push(v);
        subtree_chmin(nodes[v].left, x);
        if (nodes[v].value > x) nodes[v].value = x;
        subtree_chmin(nodes[v].right, x);
        pull(v);
    }

    void subtree_chmax(int v, i64 x) {
        if (v == -1 || nodes[v].min1 >= x) return;
        if (x < nodes[v].min2) {
            apply_chmax(v, x);
            return;
        }
        push(v);
        subtree_chmax(nodes[v].left, x);
        if (nodes[v].value < x) nodes[v].value = x;
        subtree_chmax(nodes[v].right, x);
        pull(v);
    }

    int merge(int a, int b) {
        if (a == -1) return b;
        if (b == -1) return a;
        if (nodes[a].priority < nodes[b].priority) {
            push(a);
            nodes[a].right = merge(nodes[a].right, b);
            pull(a);
            return a;
        } else {
            push(b);
            nodes[b].left = merge(a, nodes[b].left);
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
            pull(v);
            return {a, v};
        } else {
            auto [a, b] = split(nodes[v].right, k - node_size(nodes[v].left) - 1);
            nodes[v].right = a;
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

    void dfs(int v, std::vector<i64>& result) {
        if (v == -1) return;
        push(v);
        dfs(nodes[v].left, result);
        result.push_back(nodes[v].value);
        dfs(nodes[v].right, result);
    }
};
