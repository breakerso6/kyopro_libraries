#pragma once
#include <bits/stdc++.h>

template<class UInt = unsigned, int BITS = std::numeric_limits<UInt>::digits>
struct BinaryTrie {
    struct Node { int child[2] = {-1, -1}; int count = 0; };
    std::vector<Node> nodes{Node{}};

    int size() const { return nodes[0].count; }
    int count(UInt x) const {
        int v = 0;
        for (int b = BITS - 1; b >= 0; --b) {
            v = nodes[v].child[(x >> b) & 1];
            if (v == -1) return 0;
        }
        return nodes[v].count;
    }
    void insert(UInt x) {
        int v = 0; ++nodes[v].count;
        for (int b = BITS - 1; b >= 0; --b) {
            int bit = (x >> b) & 1;
            if (nodes[v].child[bit] == -1) {
                nodes[v].child[bit] = (int)nodes.size();
                nodes.push_back(Node{});
            }
            v = nodes[v].child[bit]; ++nodes[v].count;
        }
    }
    bool erase(UInt x) {
        if (!count(x)) return false;
        int v = 0; --nodes[v].count;
        for (int b = BITS - 1; b >= 0; --b) {
            v = nodes[v].child[(x >> b) & 1]; --nodes[v].count;
        }
        return true;
    }
    UInt min_xor(UInt x) const { return xor_extreme(x, false); }
    UInt max_xor(UInt x) const { return xor_extreme(x, true); }

private:
    UInt xor_extreme(UInt x, bool maximize) const {
        assert(size());
        int v = 0; UInt answer = 0;
        for (int b = BITS - 1; b >= 0; --b) {
            int bit = (x >> b) & 1, wanted = bit ^ maximize;
            int to = nodes[v].child[wanted];
            if (to == -1 || nodes[to].count == 0) wanted ^= 1;
            if (wanted != bit) answer |= UInt(1) << b;
            v = nodes[v].child[wanted];
        }
        return answer;
    }
};
