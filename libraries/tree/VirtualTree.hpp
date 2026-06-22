#pragma once
#include <bits/stdc++.h>

struct VirtualTree {
    std::vector<int> vertices;
    std::vector<int> parent;
    std::vector<long long> distance_to_parent;
    std::vector<std::vector<std::pair<int, long long>>> graph;

    template<class LCA>
    static VirtualTree build(std::vector<int> selected, LCA& lca) {
        VirtualTree result;
        if (selected.empty()) return result;
        auto by_dfs_order = [&](int a, int b) { return lca.id[a] < lca.id[b]; };
        std::sort(selected.begin(), selected.end(), by_dfs_order);
        selected.erase(std::unique(selected.begin(), selected.end()), selected.end());
        int original_size = selected.size();
        for (int i = 0; i + 1 < original_size; ++i) selected.push_back(lca.lca(selected[i], selected[i + 1]));
        std::sort(selected.begin(), selected.end(), by_dfs_order);
        selected.erase(std::unique(selected.begin(), selected.end()), selected.end());
        result.vertices = selected;
        int n = selected.size(); result.parent.assign(n, -1); result.distance_to_parent.assign(n, 0); result.graph.assign(n, {});
        std::vector<int> stack;
        for (int i = 0; i < n; ++i) {
            while (!stack.empty() && lca.id[selected[i]] >= lca.id[selected[stack.back()]] + lca.subsize[selected[stack.back()]]) stack.pop_back();
            if (!stack.empty()) {
                int p = stack.back(); result.parent[i] = p;
                long long distance = lca.depth[selected[i]] - lca.depth[selected[p]];
                result.distance_to_parent[i] = distance;
                result.graph[p].push_back({i, distance}); result.graph[i].push_back({p, distance});
            }
            stack.push_back(i);
        }
        return result;
    }
};
