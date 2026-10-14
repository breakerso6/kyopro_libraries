// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/string/AhoCorasick.hpp

#pragma once
#include <bits/stdc++.h>

template<int SIGMA = 26, char FIRST = 'a'>
struct AhoCorasick {
    struct Node {
        std::array<int, SIGMA> next;
        int link = 0;
        std::vector<int> output;
        Node() { next.fill(-1); }
    };
    std::vector<Node> nodes{Node{}};
    std::vector<int> terminal, bfs_order;
    int patterns = 0;
    size_t max_output_size = 0;

    int add(std::string_view s) {
        int v = 0;
        for (char c : s) {
            int x = c - FIRST; assert(0 <= x && x < SIGMA);
            if (nodes[v].next[x] == -1) { nodes[v].next[x] = (int)nodes.size(); nodes.push_back(Node{}); }
            v = nodes[v].next[x];
        }
        nodes[v].output.push_back(patterns);
        terminal.push_back(v);
        return patterns++;
    }
    void build() {
        std::queue<int> q;
        bfs_order.clear();
        bfs_order.reserve(nodes.size() - 1);
        max_output_size = nodes[0].output.size();
        for (int c = 0; c < SIGMA; ++c) {
            int to = nodes[0].next[c];
            if (to == -1) nodes[0].next[c] = 0;
            else q.push(to);
        }
        while (!q.empty()) {
            int v = q.front(); q.pop();
            bfs_order.push_back(v);
            const auto& inherited = nodes[nodes[v].link].output;
            nodes[v].output.insert(nodes[v].output.end(), inherited.begin(), inherited.end());
            max_output_size = std::max(max_output_size, nodes[v].output.size());
            for (int c = 0; c < SIGMA; ++c) {
                int to = nodes[v].next[c];
                if (to == -1) nodes[v].next[c] = nodes[nodes[v].link].next[c];
                else { nodes[to].link = nodes[nodes[v].link].next[c]; q.push(to); }
            }
        }
    }
    std::vector<int> count_matches(std::string_view text) const {
        // Direct enumeration avoids traversing the trie for sparse matches
        // or when even its worst-case output count is smaller than the trie.
        if (max_output_size <= 4 || text.size() <= nodes.size() / max_output_size) {
            std::vector<int> count(patterns); int v = 0;
            for (char c : text) {
                int x = c - FIRST; assert(0 <= x && x < SIGMA);
                v = nodes[v].next[x];
                for (int id : nodes[v].output) ++count[id];
            }
            return count;
        }
        std::vector<int> visits(nodes.size()); int v = 0;
        for (char c : text) {
            int x = c - FIRST; assert(0 <= x && x < SIGMA);
            v = nodes[v].next[x];
            ++visits[v];
        }
        // Every visit also matches the suffixes on the failure-link path.
        // Aggregate once per node rather than enumerate every occurrence.
        for (auto it = bfs_order.rbegin(); it != bfs_order.rend(); ++it)
            visits[nodes[*it].link] += visits[*it];
        std::vector<int> count(patterns);
        for (int id = 0; id < patterns; ++id) count[id] = visits[terminal[id]];
        return count;
    }
};
