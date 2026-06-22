#pragma once
#include <bits/stdc++.h>

struct CentroidDecomposition {
    int n;
    const std::vector<std::vector<int>>& graph;
    std::vector<int> parent, depth, subtree;
    std::vector<char> removed;

    explicit CentroidDecomposition(const std::vector<std::vector<int>>& graph_)
        : n((int)graph_.size()), graph(graph_), parent(n, -1), depth(n), subtree(n), removed(n) {
        if (n) decompose(0, -1, 0);
    }

private:
    int calculate_size(int v, int p) {
        subtree[v] = 1;
        for (int to : graph[v]) if (to != p && !removed[to]) subtree[v] += calculate_size(to, v);
        return subtree[v];
    }
    int find_centroid(int v, int p, int total) {
        for (int to : graph[v])
            if (to != p && !removed[to] && subtree[to] > total / 2) return find_centroid(to, v, total);
        return v;
    }
    void decompose(int entry, int p, int d) {
        int c = find_centroid(entry, -1, calculate_size(entry, -1));
        parent[c] = p; depth[c] = d; removed[c] = 1;
        for (int to : graph[c]) if (!removed[to]) decompose(to, c, d + 1);
    }
};
