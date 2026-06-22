#pragma once
#include <bits/stdc++.h>

struct PalindromicTree {
    struct Node {
        int length, suffix_link, first_end = -1;
        long long occurrences = 0;
        std::map<char, int> next;
    };
    std::string text;
    std::vector<Node> nodes;
    int suffix = 1;
    bool propagated = false;

    PalindromicTree() { clear(); }
    explicit PalindromicTree(const std::string& s) { clear(); for (char c : s) add(c); }
    void clear() {
        text.clear(); nodes.clear();
        nodes.push_back(Node{-1, 0, -1, 0, {}}); nodes.push_back(Node{0, 0, -1, 0, {}});
        suffix = 1; propagated = false;
    }
    int add(char c) {
        if (propagated) throw std::logic_error("cannot add after propagating occurrences");
        int position = text.size(); text.push_back(c);
        int current = find_suffix(suffix, position);
        auto it = nodes[current].next.find(c);
        if (it != nodes[current].next.end()) {
            suffix = it->second; ++nodes[suffix].occurrences; return suffix;
        }
        int created = nodes.size();
        nodes.push_back(Node{nodes[current].length + 2, 0, position, 1, {}});
        nodes[current].next[c] = created;
        if (nodes[created].length == 1) nodes[created].suffix_link = 1;
        else {
            int link_from = find_suffix(nodes[current].suffix_link, position);
            nodes[created].suffix_link = nodes[link_from].next[c];
        }
        suffix = created; return created;
    }
    int distinct_count() const { return (int)nodes.size() - 2; }
    void propagate_occurrences() {
        if (propagated) return;
        std::vector<int> order(nodes.size() - 2);
        std::iota(order.begin(), order.end(), 2);
        std::sort(order.begin(), order.end(), [&](int a, int b) { return nodes[a].length > nodes[b].length; });
        for (int v : order) nodes[nodes[v].suffix_link].occurrences += nodes[v].occurrences;
        propagated = true;
    }
    std::string palindrome(int node) const {
        assert(2 <= node && node < (int)nodes.size());
        int length = nodes[node].length;
        return text.substr(nodes[node].first_end - length + 1, length);
    }

private:
    int find_suffix(int node, int position) const {
        while (position - 1 - nodes[node].length < 0 ||
               text[position - 1 - nodes[node].length] != text[position])
            node = nodes[node].suffix_link;
        return node;
    }
};
