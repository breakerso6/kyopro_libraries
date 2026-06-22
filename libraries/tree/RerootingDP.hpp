#pragma once
#include <bits/stdc++.h>

// merge must be associative and preserve adjacency-list order.
template<class DP, class EdgeData, class Merge, class AddVertex, class AddEdge>
struct RerootingDP {
    struct Edge { int to, reverse; EdgeData data; };
    int n;
    DP identity;
    Merge merge;
    AddVertex add_vertex;
    AddEdge add_edge_dp;
    std::vector<std::vector<Edge>> graph;
    std::vector<DP> answer;

    RerootingDP(int n_, DP identity_, Merge merge_, AddVertex add_vertex_, AddEdge add_edge_dp_)
        : n(n_), identity(identity_), merge(merge_), add_vertex(add_vertex_),
          add_edge_dp(add_edge_dp_), graph(n_) {}

    void add_edge(int u, int v, const EdgeData& uv, const EdgeData& vu) {
        int ui = (int)graph[u].size(), vi = (int)graph[v].size();
        graph[u].push_back({v, vi, uv});
        graph[v].push_back({u, ui, vu});
    }
    void add_edge(int u, int v, const EdgeData& data = EdgeData()) { add_edge(u, v, data, data); }

    std::vector<DP> build(int root = 0) {
        if (n == 0) return answer = {};
        std::vector<int> parent(n, -2), order{root};
        parent[root] = -1;
        for (int i = 0; i < (int)order.size(); ++i) {
            int v = order[i];
            for (int j = 0; j < (int)graph[v].size(); ++j) {
                int to = graph[v][j].to;
                if (parent[to] != -2) continue;
                parent[to] = v; order.push_back(to);
            }
        }
        assert((int)order.size() == n);

        std::vector<DP> down(n, identity), from_parent(n, identity);
        for (int oi = n - 1; oi >= 0; --oi) {
            int v = order[oi]; DP combined = identity;
            for (const Edge& edge : graph[v]) if (edge.to != parent[v])
                combined = merge(combined, add_edge_dp(down[edge.to], edge.data, edge.to, v));
            down[v] = add_vertex(combined, v);
        }

        answer.resize(n);
        for (int v : order) {
            int degree = (int)graph[v].size();
            std::vector<DP> contribution(degree), prefix(degree + 1, identity), suffix(degree + 1, identity);
            for (int i = 0; i < degree; ++i) {
                const Edge& edge = graph[v][i];
                contribution[i] = edge.to == parent[v]
                    ? from_parent[v]
                    : add_edge_dp(down[edge.to], edge.data, edge.to, v);
                prefix[i + 1] = merge(prefix[i], contribution[i]);
            }
            for (int i = degree - 1; i >= 0; --i) suffix[i] = merge(contribution[i], suffix[i + 1]);
            answer[v] = add_vertex(prefix[degree], v);
            for (int i = 0; i < degree; ++i) {
                const Edge& edge = graph[v][i];
                if (edge.to == parent[v]) continue;
                DP without_child = add_vertex(merge(prefix[i], suffix[i + 1]), v);
                const Edge& reverse = graph[edge.to][edge.reverse];
                from_parent[edge.to] = add_edge_dp(without_child, reverse.data, v, edge.to);
            }
        }
        return answer;
    }
};

template<class DP, class EdgeData, class Merge, class AddVertex, class AddEdge>
auto make_rerooting_dp(int n, DP identity, Merge merge, AddVertex add_vertex, AddEdge add_edge_dp) {
    return RerootingDP<DP, EdgeData, Merge, AddVertex, AddEdge>(
        n, std::move(identity), std::move(merge), std::move(add_vertex), std::move(add_edge_dp));
}
