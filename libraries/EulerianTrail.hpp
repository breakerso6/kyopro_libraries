#pragma once
#include <bits/stdc++.h>

struct EulerianTrail {
    struct Result { std::vector<int> vertices, edge_ids; };

    static std::optional<Result> undirected(int n, const std::vector<std::pair<int, int>>& edges) {
        int m = (int)edges.size();
        std::vector<std::vector<std::pair<int, int>>> g(n);
        std::vector<int> degree(n);
        for (int i = 0; i < m; ++i) {
            auto [u, v] = edges[i];
            g[u].push_back({v, i}); g[v].push_back({u, i}); ++degree[u]; ++degree[v];
        }
        std::vector<int> odd;
        for (int v = 0; v < n; ++v) if (degree[v] & 1) odd.push_back(v);
        if (odd.size() != 0 && odd.size() != 2) return std::nullopt;
        int start = odd.empty() ? first_nonzero(degree) : odd[0];
        return run(g, m, start);
    }
    static std::optional<Result> directed(int n, const std::vector<std::pair<int, int>>& edges) {
        int m = (int)edges.size();
        std::vector<std::vector<std::pair<int, int>>> g(n);
        std::vector<int> in(n), out(n);
        for (int i = 0; i < m; ++i) { auto [u, v] = edges[i]; g[u].push_back({v, i}); ++out[u]; ++in[v]; }
        int start = -1, finish = -1;
        for (int v = 0; v < n; ++v) {
            if (out[v] - in[v] == 1) { if (start != -1) return std::nullopt; start = v; }
            else if (in[v] - out[v] == 1) { if (finish != -1) return std::nullopt; finish = v; }
            else if (in[v] != out[v]) return std::nullopt;
        }
        if ((start == -1) != (finish == -1)) return std::nullopt;
        if (start == -1) start = first_nonzero(out);
        return run(g, m, start);
    }

private:
    static int first_nonzero(const std::vector<int>& degree) {
        for (int i = 0; i < (int)degree.size(); ++i) if (degree[i]) return i;
        return degree.empty() ? -1 : 0;
    }
    static std::optional<Result> run(const std::vector<std::vector<std::pair<int, int>>>& g, int m, int start) {
        if (start == -1) return Result{};
        std::vector<int> ptr(g.size()), used(m), vertices, edge_ids;
        std::vector<std::pair<int, int>> stack{{start, -1}};
        while (!stack.empty()) {
            int v = stack.back().first;
            while (ptr[v] < (int)g[v].size() && used[g[v][ptr[v]].second]) ++ptr[v];
            if (ptr[v] == (int)g[v].size()) {
                vertices.push_back(v); edge_ids.push_back(stack.back().second); stack.pop_back();
            } else {
                auto [to, id] = g[v][ptr[v]++];
                if (!used[id]) { used[id] = 1; stack.push_back({to, id}); }
            }
        }
        if ((int)vertices.size() != m + 1) return std::nullopt;
        std::reverse(vertices.begin(), vertices.end());
        std::reverse(edge_ids.begin(), edge_ids.end());
        edge_ids.erase(edge_ids.begin());
        return Result{vertices, edge_ids};
    }
};
