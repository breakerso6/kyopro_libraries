#pragma once
#include <bits/stdc++.h>

struct TwoEdgeConnectedComponents {
    int n;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::vector<std::pair<int, int>>> graph;
    std::vector<int> component;
    std::vector<std::vector<int>> groups, tree;
    std::vector<bool> is_bridge;

    explicit TwoEdgeConnectedComponents(int n_) : n(n_), graph(n_) {}
    int add_edge(int u, int v) {
        assert(u != v); int id = edges.size(); edges.push_back({u, v});
        graph[u].push_back({v, id}); graph[v].push_back({u, id}); return id;
    }
    void build() {
        int m = edges.size(), timer = 0;
        std::vector<int> order(n, -1), low(n);
        is_bridge.assign(m, false);
        std::function<void(int, int)> dfs = [&](int v, int parent_edge) {
            order[v] = low[v] = timer++;
            for (auto [to, id] : graph[v]) {
                if (id == parent_edge) continue;
                if (order[to] == -1) {
                    dfs(to, id); low[v] = std::min(low[v], low[to]);
                    if (order[v] < low[to]) is_bridge[id] = true;
                } else low[v] = std::min(low[v], order[to]);
            }
        };
        for (int v = 0; v < n; ++v) if (order[v] == -1) dfs(v, -1);
        component.assign(n, -1); groups.clear();
        for (int start = 0; start < n; ++start) if (component[start] == -1) {
            int id = groups.size(); groups.push_back({});
            std::vector<int> stack{start}; component[start] = id;
            while (!stack.empty()) {
                int v = stack.back(); stack.pop_back(); groups[id].push_back(v);
                for (auto [to, edge_id] : graph[v]) if (!is_bridge[edge_id] && component[to] == -1)
                    component[to] = id, stack.push_back(to);
            }
        }
        tree.assign(groups.size(), {});
        for (int id = 0; id < m; ++id) if (is_bridge[id]) {
            auto [u, v] = edges[id]; int a = component[u], b = component[v];
            tree[a].push_back(b); tree[b].push_back(a);
        }
    }
};

struct VertexBiconnectedComponents {
    int n;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::vector<std::pair<int, int>>> graph;
    std::vector<std::vector<int>> components, block_cut_tree;
    std::vector<int> articulation, vertex_node;

    explicit VertexBiconnectedComponents(int n_) : n(n_), graph(n_) {}
    int add_edge(int u, int v) {
        assert(u != v); int id = edges.size(); edges.push_back({u, v});
        graph[u].push_back({v, id}); graph[v].push_back({u, id}); return id;
    }
    void build() {
        std::vector<int> order(n, -1), low(n), edge_stack;
        std::vector<bool> is_articulation(n, false); components.clear();
        int timer = 0;
        std::function<void(int, int)> dfs = [&](int v, int parent_edge) {
            order[v] = low[v] = timer++; int children = 0;
            for (auto [to, id] : graph[v]) {
                if (id == parent_edge) continue;
                if (order[to] == -1) {
                    ++children; edge_stack.push_back(id); dfs(to, id); low[v] = std::min(low[v], low[to]);
                    if (low[to] >= order[v]) {
                        if (parent_edge != -1 || children >= 2) is_articulation[v] = true;
                        std::vector<int> vertices;
                        while (true) {
                            int edge_id = edge_stack.back(); edge_stack.pop_back();
                            vertices.push_back(edges[edge_id].first); vertices.push_back(edges[edge_id].second);
                            if (edge_id == id) break;
                        }
                        std::sort(vertices.begin(), vertices.end());
                        vertices.erase(std::unique(vertices.begin(), vertices.end()), vertices.end());
                        components.push_back(std::move(vertices));
                    }
                } else {
                    low[v] = std::min(low[v], order[to]);
                    if (order[to] < order[v]) edge_stack.push_back(id);
                }
            }
            if (parent_edge == -1 && children < 2) is_articulation[v] = false;
        };
        for (int v = 0; v < n; ++v) if (order[v] == -1) {
            if (graph[v].empty()) { order[v] = low[v] = timer++; components.push_back({v}); }
            else dfs(v, -1);
        }
        articulation.clear();
        for (int v = 0; v < n; ++v) if (is_articulation[v]) articulation.push_back(v);
        int blocks = components.size();
        std::vector<int> articulation_node(n, -1);
        for (int i = 0; i < (int)articulation.size(); ++i) articulation_node[articulation[i]] = blocks + i;
        block_cut_tree.assign(blocks + articulation.size(), {}); vertex_node.assign(n, -1);
        for (int block = 0; block < blocks; ++block) for (int v : components[block]) {
            if (is_articulation[v]) {
                int node = articulation_node[v]; block_cut_tree[block].push_back(node); block_cut_tree[node].push_back(block);
            } else vertex_node[v] = block;
        }
        for (int v : articulation) vertex_node[v] = articulation_node[v];
    }
};
