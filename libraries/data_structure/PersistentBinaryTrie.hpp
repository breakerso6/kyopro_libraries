#pragma once
#include <bits/stdc++.h>

template<class UInt = unsigned, int BITS = std::numeric_limits<UInt>::digits>
struct PersistentBinaryTrie {
    struct Node { int child[2] = {-1, -1}; int count = 0; };
    std::vector<Node> nodes{Node{}};

    int empty_root() const { return 0; }
    int size(int root) const { return nodes[root].count; }
    int insert(int root, UInt value) { return add(root, value, 1, BITS - 1); }
    int erase(int root, UInt value) {
        assert(count(root, value) > 0);
        return add(root, value, -1, BITS - 1);
    }
    int count(int root, UInt value) const {
        int v = root;
        for (int bit = BITS - 1; bit >= 0 && v != -1; --bit) v = nodes[v].child[(value >> bit) & 1];
        return v == -1 ? 0 : nodes[v].count;
    }
    // k-th smallest value of (stored_value xor x) in multiset right-left, 0-indexed.
    UInt kth_xor(int left_root, int right_root, UInt x, int k) const {
        int total = nodes[right_root].count - nodes[left_root].count;
        assert(0 <= k && k < total);
        int left = left_root, right = right_root; UInt result = 0;
        for (int bit = BITS - 1; bit >= 0; --bit) {
            int preferred = (x >> bit) & 1;
            int left_child = nodes[left].child[preferred], right_child = nodes[right].child[preferred];
            int preferred_count = node_count(right_child) - node_count(left_child);
            int chosen = preferred;
            if (k >= preferred_count) { k -= preferred_count; chosen ^= 1; result |= UInt(1) << bit; }
            left = nodes[left].child[chosen]; right = nodes[right].child[chosen];
            if (left == -1) left = 0;
        }
        return result;
    }
    UInt min_xor(int left_root, int right_root, UInt x) const { return kth_xor(left_root, right_root, x, 0); }
    UInt max_xor(int left_root, int right_root, UInt x) const {
        return kth_xor(left_root, right_root, x, nodes[right_root].count - nodes[left_root].count - 1);
    }

private:
    int node_count(int v) const { return v == -1 ? 0 : nodes[v].count; }
    int clone_or_empty(int v) { nodes.push_back(v == -1 ? Node{} : nodes[v]); return (int)nodes.size() - 1; }
    int add(int v, UInt value, int delta, int bit) {
        int result = clone_or_empty(v); nodes[result].count += delta;
        if (bit < 0) return result;
        int direction = (value >> bit) & 1;
        nodes[result].child[direction] = add(nodes[result].child[direction], value, delta, bit - 1);
        return result;
    }
};
