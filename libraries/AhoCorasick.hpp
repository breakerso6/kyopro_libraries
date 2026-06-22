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
    int patterns = 0;

    int add(std::string_view s) {
        int v = 0;
        for (char c : s) {
            int x = c - FIRST; assert(0 <= x && x < SIGMA);
            if (nodes[v].next[x] == -1) { nodes[v].next[x] = (int)nodes.size(); nodes.push_back(Node{}); }
            v = nodes[v].next[x];
        }
        nodes[v].output.push_back(patterns);
        return patterns++;
    }
    void build() {
        std::queue<int> q;
        for (int c = 0; c < SIGMA; ++c) {
            int to = nodes[0].next[c];
            if (to == -1) nodes[0].next[c] = 0;
            else q.push(to);
        }
        while (!q.empty()) {
            int v = q.front(); q.pop();
            auto inherited = nodes[nodes[v].link].output;
            nodes[v].output.insert(nodes[v].output.end(), inherited.begin(), inherited.end());
            for (int c = 0; c < SIGMA; ++c) {
                int to = nodes[v].next[c];
                if (to == -1) nodes[v].next[c] = nodes[nodes[v].link].next[c];
                else { nodes[to].link = nodes[nodes[v].link].next[c]; q.push(to); }
            }
        }
    }
    std::vector<int> count_matches(std::string_view text) const {
        std::vector<int> count(patterns); int v = 0;
        for (char c : text) {
            int x = c - FIRST; assert(0 <= x && x < SIGMA);
            v = nodes[v].next[x];
            for (int id : nodes[v].output) ++count[id];
        }
        return count;
    }
};
