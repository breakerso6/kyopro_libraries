// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/graph/Graph.hpp

#pragma once
#include <bits/stdc++.h>

struct GraphPath {
    std::vector<int> vertices;
    std::vector<int> edge_ids;
};

template<class Cost = long long, bool Directed = false>
struct Graph {
    struct Edge {
        int from, to;
        Cost cost;
        int id;
    };
    static constexpr bool directed = Directed;
    int n;
    // Each logical edge occurs once in edges, and once/twice in adj.
    // An undirected self-loop contributes two arcs with the same edge id.
    std::vector<Edge> edges;
    std::vector<std::vector<Edge>> adj;

    explicit Graph(int n_ = 0) : n(n_) {
        assert(n >= 0);
        adj.resize(n);
    }
    Graph(int n_, const std::vector<std::tuple<int, int, Cost>>& input) : Graph(n_) {
        std::vector<int> degree(n);
        for (const auto& item : input) {
            int from = std::get<0>(item), to = std::get<1>(item);
            check_vertex(from); check_vertex(to);
            ++degree[from];
            if constexpr (!Directed) ++degree[to];
        }
        edges.reserve(input.size());
        for (int v = 0; v < n; ++v) adj[v].reserve(degree[v]);
        for (const auto& [from, to, cost] : input) add_edge(from, to, cost);
    }
    int size() const { return n; }
    int edge_count() const { return (int)edges.size(); }
    const std::vector<Edge>& operator[](int v) const { check_vertex(v); return adj[v]; }
    int degree(int v) const { return (int)(*this)[v].size(); }

    int add_edge(int from, int to, Cost cost = Cost(1)) {
        check_vertex(from); check_vertex(to);
        int id = edge_count();
        edges.push_back({from, to, cost, id});
        adj[from].push_back({from, to, cost, id});
        if constexpr (!Directed) adj[to].push_back({to, from, cost, id});
        return id;
    }

    // Input vertices are shifted by index_offset (1 for usual contest input).
    // Convenient form: read_edges(m, 0) or read_edges<true>(m, 1).
    template<bool Weighted = false>
    void read_edges(int count, int index_offset, std::istream& input = std::cin) {
        read_edges<Weighted>(count, input, index_offset);
    }

    template<bool Weighted = false>
    void read_edges(int count, std::istream& input = std::cin, int index_offset = 1) {
        assert(count >= 0);
        for (int i = 0; i < count; ++i) {
            int from, to;
            Cost cost = Cost(1);
            if (!(input >> from >> to)) throw std::runtime_error("invalid graph edge input");
            if constexpr (Weighted)
                if (!(input >> cost)) throw std::runtime_error("invalid graph cost input");
            add_edge(from - index_offset, to - index_offset, cost);
        }
    }

    std::vector<int> indegree() const {
        std::vector<int> result(n);
        for (const auto& row : adj) for (const auto& e : row) ++result[e.to];
        return result;
    }

    Graph reversed() const {
        Graph result(n);
        result.edges.reserve(edges.size());
        auto degree = indegree();
        for (int v = 0; v < n; ++v) result.adj[v].reserve(degree[v]);
        for (const auto& e : edges) result.add_edge(e.to, e.from, e.cost);
        return result;
    }

    template<class Index = int>
    std::vector<std::vector<Index>> unweighted_adjacency() const {
        std::vector<std::vector<Index>> result(n);
        for (int v = 0; v < n; ++v) {
            result[v].reserve(adj[v].size());
            for (const auto& e : adj[v]) result[v].push_back(Index(e.to));
        }
        return result;
    }

private:
    void check_vertex(int v) const { assert(0 <= v && v < n); (void)v; }
};
