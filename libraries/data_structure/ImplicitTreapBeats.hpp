#pragma once
#include <bits/stdc++.h>

struct ImplicitTreapBeats {
    using i64 = long long;
    using Handle = int;
    static constexpr i64 INF = std::numeric_limits<i64>::max() / 4;

    struct Node {
        i64 value = 0;
        i64 sum = 0, add = 0;
        i64 max1 = -INF, max2 = -INF, min1 = INF, min2 = INF;
        int max_count = 0, min_count = 0;
        int left = -1, right = -1, parent = -1, size = 1;
        uint32_t priority = 0;
        bool rev = false;
        bool active = true;
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
        std::vector<int> stack;
        stack.reserve(values.size());
        for (i64 value : values) {
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

    Handle insert(int position, i64 value) {
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
        reset_node(b, nodes[b].value);
        nodes[b].active = false;
        return b;
    }

    void reinsert(int position, Handle handle) {
        assert(0 <= position && position <= size());
        assert(valid_handle(handle) && !nodes[handle].active);
        reset_node(handle, nodes[handle].value);
        nodes[handle].active = true;
        auto [a, b] = split(root, position);
        root = merge(merge(a, handle), b);
    }

    void reinsert(int position, Handle handle, i64 value) {
        assert(valid_handle(handle) && !nodes[handle].active);
        reset_node(handle, value);
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

    std::vector<Handle> to_handles() {
        std::vector<Handle> result;
        result.reserve(size());
        dfs_handles(root, result);
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
        nodes[v].active = true;
        reset_node(v, value);
        return v;
    }

    bool valid_handle(Handle handle) const {
        return 0 <= handle && handle < (int)nodes.size();
    }

    void reset_node(int v, i64 value) {
        int left = nodes[v].left, right = nodes[v].right, parent = nodes[v].parent;
        uint32_t priority = nodes[v].priority;
        bool active = nodes[v].active;
        nodes[v] = Node();
        nodes[v].left = left;
        nodes[v].right = right;
        nodes[v].parent = parent;
        nodes[v].priority = priority;
        nodes[v].active = active;
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
        int left = nodes[v].left, right = nodes[v].right, parent = nodes[v].parent;
        uint32_t priority = nodes[v].priority;
        bool rev = nodes[v].rev;
        bool active = nodes[v].active;
        nodes[v] = info;
        nodes[v].value = value;
        nodes[v].add = add;
        nodes[v].left = left;
        nodes[v].right = right;
        nodes[v].parent = parent;
        nodes[v].priority = priority;
        nodes[v].rev = rev;
        nodes[v].active = active;
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

    void push_path(int v) {
        std::vector<int> path;
        for (int u = v; u != -1; u = nodes[u].parent) path.push_back(u);
        std::reverse(path.begin(), path.end());
        for (int u : path) push(u);
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

    void dfs(int v, std::vector<i64>& result) {
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
