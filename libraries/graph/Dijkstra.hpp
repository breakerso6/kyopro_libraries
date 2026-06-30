#pragma once
#include <bits/stdc++.h>

template<class T>
struct Dijkstra {
    struct Edge { int to; T cost; };
    int n;
    std::vector<std::vector<Edge>> graph;
    explicit Dijkstra(int n_) : n(n_), graph(n_) {}
    void add_edge(int from, int to, T cost, bool directed = true) {
        assert(0 <= from && from < n && 0 <= to && to < n);
        graph[from].push_back({to, cost});
        if (!directed) graph[to].push_back({from, cost});
    }
    std::vector<T> shortest_path(int source, T infinity) const {
        assert(0 <= source && source < n);
        std::vector<T> dist(n, infinity);
        using P = std::pair<T, int>;
        std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
        dist[source] = 0; pq.push({0, source});
        while (!pq.empty()) {
            auto [d, v] = pq.top(); pq.pop();
            if (d != dist[v]) continue;
            for (auto e : graph[v]) if (dist[e.to] > d + e.cost) {
                dist[e.to] = d + e.cost;
                pq.push({dist[e.to], e.to});
            }
        }
        return dist;
    }
};
