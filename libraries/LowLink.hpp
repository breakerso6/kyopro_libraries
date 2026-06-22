#pragma once
#include <bits/stdc++.h>

struct LowLink {
    int n;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::vector<std::pair<int, int>>> graph;
    std::vector<int> order, low, articulation;
    std::vector<std::pair<int, int>> bridges;

    explicit LowLink(int n_) : n(n_), graph(n_) {}
    int add_edge(int u, int v) {
        int id = (int)edges.size();
        edges.push_back({u, v});
        graph[u].push_back({v, id}); graph[v].push_back({u, id});
        return id;
    }
    void build() {
        order.assign(n, -1); low.resize(n); articulation.clear(); bridges.clear();
        int timer = 0;
        std::function<void(int, int)> dfs = [&](int v, int parent_edge) {
            order[v] = low[v] = timer++;
            int children = 0; bool is_articulation = false;
            for (auto [to, id] : graph[v]) {
                if (id == parent_edge) continue;
                if (order[to] == -1) {
                    ++children; dfs(to, id); low[v] = std::min(low[v], low[to]);
                    if (parent_edge != -1 && order[v] <= low[to]) is_articulation = true;
                    if (order[v] < low[to]) bridges.push_back(edges[id]);
                } else {
                    low[v] = std::min(low[v], order[to]);
                }
            }
            if (parent_edge == -1 && children >= 2) is_articulation = true;
            if (is_articulation) articulation.push_back(v);
        };
        for (int v = 0; v < n; ++v) if (order[v] == -1) dfs(v, -1);
    }
};
