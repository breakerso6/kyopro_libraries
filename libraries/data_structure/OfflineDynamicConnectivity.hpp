#pragma once
#include <bits/stdc++.h>
#include "libraries/data_structure/RollbackDSU.hpp"

struct OfflineDynamicConnectivity {
    enum QueryType { SAME, SIZE, COMPONENTS };
    struct Query { QueryType type; int u, v, id; };

    int n, time = 0, query_count = 0;
    std::map<std::pair<int, int>, std::vector<int>> active;
    std::vector<std::tuple<int, int, std::pair<int, int>>> intervals;
    std::vector<std::vector<Query>> queries;

    explicit OfflineDynamicConnectivity(int n_) : n(n_) {}

    void add_edge(int u, int v) {
        normalize(u, v); active[{u, v}].push_back(time++); queries.emplace_back();
    }
    void remove_edge(int u, int v) {
        normalize(u, v);
        auto& starts = active[{u, v}];
        if (starts.empty()) throw std::invalid_argument("removing an inactive edge");
        intervals.push_back({starts.back(), time, {u, v}}); starts.pop_back();
        ++time; queries.emplace_back();
    }
    int same_query(int u, int v) { return add_query({SAME, u, v, query_count++}); }
    int size_query(int v) { return add_query({SIZE, v, -1, query_count++}); }
    int components_query() { return add_query({COMPONENTS, -1, -1, query_count++}); }

    std::vector<long long> run() const {
        int q = time;
        std::vector<std::tuple<int, int, std::pair<int, int>>> all = intervals;
        for (const auto& [edge, starts] : active)
            for (int start : starts) all.push_back({start, q, edge});
        int base = 1; while (base < std::max(q, 1)) base <<= 1;
        std::vector<std::vector<std::pair<int, int>>> seg(2 * base);
        for (auto [l, r, edge] : all) {
            for (l += base, r += base; l < r; l >>= 1, r >>= 1) {
                if (l & 1) seg[l++].push_back(edge);
                if (r & 1) seg[--r].push_back(edge);
            }
        }
        RollbackDSU dsu(n);
        std::vector<long long> answer(query_count);
        std::function<void(int)> dfs = [&](int node) {
            int state = dsu.snapshot();
            for (auto [u, v] : seg[node]) dsu.merge(u, v);
            if (node >= base) {
                int t = node - base;
                if (t < q) for (const Query& query : queries[t]) {
                    if (query.type == SAME) answer[query.id] = dsu.same(query.u, query.v);
                    else if (query.type == SIZE) answer[query.id] = dsu.size(query.u);
                    else answer[query.id] = dsu.components;
                }
            } else {
                dfs(2 * node); dfs(2 * node + 1);
            }
            dsu.rollback(state);
        };
        dfs(1);
        return answer;
    }

private:
    static void normalize(int& u, int& v) { if (u > v) std::swap(u, v); }
    int add_query(Query query) {
        int id = query.id; queries.emplace_back(); queries.back().push_back(query); ++time; return id;
    }
};
