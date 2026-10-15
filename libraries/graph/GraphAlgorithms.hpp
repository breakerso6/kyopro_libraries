// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/graph/GraphAlgorithms.hpp
// SPDX-License-Identifier: CC0-1.0

#pragma once
#include "libraries/graph/Graph.hpp"

template<class Distance>
struct GraphSearchResult {
    std::vector<Distance> distance;
    std::vector<int> parent, parent_edge, order;
    std::vector<char> reached;

    std::optional<GraphPath> path_to(int target) const {
        assert(0 <= target && target < (int)reached.size());
        if (!reached[target]) return std::nullopt;
        GraphPath path;
        for (int v = target; v != -1; v = parent[v]) {
            path.vertices.push_back(v);
            if (parent[v] != -1) path.edge_ids.push_back(parent_edge[v]);
        }
        std::reverse(path.vertices.begin(), path.vertices.end());
        std::reverse(path.edge_ids.begin(), path.edge_ids.end());
        return path;
    }
};

template<class Cost, bool Directed>
GraphSearchResult<int> breadth_first_search(const Graph<Cost, Directed>& graph, const std::vector<int>& sources) {
    GraphSearchResult<int> result;
    result.distance.assign(graph.size(), -1);
    result.parent.assign(graph.size(), -1); result.parent_edge.assign(graph.size(), -1);
    result.reached.assign(graph.size(), false); result.order.reserve(graph.size());
    for (int source : sources) {
        assert(0 <= source && source < graph.size());
        if (result.reached[source]) continue;
        result.reached[source] = true; result.distance[source] = 0;
        result.order.push_back(source);
    }
    for (size_t i = 0; i < result.order.size(); ++i) {
        int v = result.order[i];
        for (const auto& e : graph[v]) if (!result.reached[e.to]) {
            result.reached[e.to] = true; result.distance[e.to] = result.distance[v] + 1;
            result.parent[e.to] = v; result.parent_edge[e.to] = e.id;
            result.order.push_back(e.to);
        }
    }
    return result;
}

template<class Cost, bool Directed>
GraphSearchResult<int> breadth_first_search(const Graph<Cost, Directed>& graph, int source) {
    return breadth_first_search(graph, std::vector<int>{source});
}

template<class Cost, bool Directed>
GraphSearchResult<int> zero_one_bfs(const Graph<Cost, Directed>& graph, const std::vector<int>& sources) {
    GraphSearchResult<int> result;
    result.distance.assign(graph.size(), -1);
    result.parent.assign(graph.size(), -1); result.parent_edge.assign(graph.size(), -1);
    result.reached.assign(graph.size(), false); result.order.reserve(graph.size());
    std::deque<std::pair<int, int>> queue;
    for (int source : sources) {
        assert(0 <= source && source < graph.size());
        if (result.reached[source]) continue;
        result.reached[source] = true; result.distance[source] = 0;
        queue.push_back({0, source});
    }
    while (!queue.empty()) {
        auto [distance, v] = queue.front(); queue.pop_front();
        if (distance != result.distance[v]) continue;
        result.order.push_back(v);
        for (const auto& e : graph[v]) {
            assert(e.cost == Cost(0) || e.cost == Cost(1));
            int weight = e.cost == Cost(0) ? 0 : 1, next = distance + weight;
            if (result.reached[e.to] && result.distance[e.to] <= next) continue;
            result.reached[e.to] = true; result.distance[e.to] = next;
            result.parent[e.to] = v; result.parent_edge[e.to] = e.id;
            if (weight) queue.push_back({next, e.to});
            else queue.push_front({next, e.to});
        }
    }
    return result;
}

template<class Cost, bool Directed>
GraphSearchResult<int> zero_one_bfs(const Graph<Cost, Directed>& graph, int source) {
    return zero_one_bfs(graph, std::vector<int>{source});
}

// Distances greater than infinity are outside the representable search range.
// reached distinguishes an unreachable vertex from a distance == infinity.
template<class Cost, bool Directed>
GraphSearchResult<Cost> dijkstra(const Graph<Cost, Directed>& graph, const std::vector<int>& sources, Cost infinity) {
    assert(!(infinity < Cost(0)));
    GraphSearchResult<Cost> result;
    result.distance.assign(graph.size(), infinity);
    result.parent.assign(graph.size(), -1); result.parent_edge.assign(graph.size(), -1);
    result.reached.assign(graph.size(), false); result.order.reserve(graph.size());
    using Item = std::pair<Cost, int>;
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> queue;
    for (int source : sources) {
        assert(0 <= source && source < graph.size());
        if (result.reached[source]) continue;
        result.reached[source] = true; result.distance[source] = Cost(0);
        queue.push({Cost(0), source});
    }
    while (!queue.empty()) {
        auto [distance, v] = queue.top(); queue.pop();
        if (distance != result.distance[v]) continue;
        result.order.push_back(v);
        for (const auto& e : graph[v]) {
            assert(!(e.cost < Cost(0)));
            // Subtract before adding to avoid signed/unsigned overflow.
            if (infinity - distance < e.cost) continue;
            Cost next = distance + e.cost;
            if (result.reached[e.to] && !(next < result.distance[e.to])) continue;
            result.reached[e.to] = true; result.distance[e.to] = next;
            result.parent[e.to] = v; result.parent_edge[e.to] = e.id;
            queue.push({next, e.to});
        }
    }
    return result;
}

template<class Cost, bool Directed>
GraphSearchResult<Cost> dijkstra(const Graph<Cost, Directed>& graph, int source, Cost infinity) {
    return dijkstra(graph, std::vector<int>{source}, infinity);
}

template<class Cost>
std::optional<std::vector<int>> topological_sort(const Graph<Cost, true>& graph) {
    auto indegree = graph.indegree();
    std::vector<int> order; order.reserve(graph.size());
    for (int v = 0; v < graph.size(); ++v) if (indegree[v] == 0) order.push_back(v);
    for (size_t i = 0; i < order.size(); ++i)
        for (const auto& e : graph[order[i]]) if (--indegree[e.to] == 0) order.push_back(e.to);
    if ((int)order.size() != graph.size()) return std::nullopt;
    return order;
}

struct GraphComponents {
    std::vector<int> component;
    std::vector<std::vector<int>> groups;
    int size() const { return (int)groups.size(); }
};

template<class Cost>
GraphComponents connected_components(const Graph<Cost, false>& graph) {
    GraphComponents result;
    result.component.assign(graph.size(), -1);
    for (int root = 0; root < graph.size(); ++root) if (result.component[root] == -1) {
        int id = result.size(); result.groups.push_back({root}); result.component[root] = id;
        auto& group = result.groups.back();
        for (size_t i = 0; i < group.size(); ++i)
            for (const auto& e : graph[group[i]]) if (result.component[e.to] == -1) {
                result.component[e.to] = id; group.push_back(e.to);
            }
    }
    return result;
}

template<class Cost>
std::optional<std::vector<int>> bipartite_coloring(const Graph<Cost, false>& graph) {
    std::vector<int> color(graph.size(), -1), queue; queue.reserve(graph.size());
    for (int root = 0; root < graph.size(); ++root) if (color[root] == -1) {
        queue.clear(); queue.push_back(root); color[root] = 0;
        for (size_t i = 0; i < queue.size(); ++i) {
            int v = queue[i];
            for (const auto& e : graph[v]) {
                if (color[e.to] == color[v]) return std::nullopt;
                if (color[e.to] == -1) { color[e.to] = color[v] ^ 1; queue.push_back(e.to); }
            }
        }
    }
    return color;
}

struct GraphSCC : GraphComponents {
    Graph<int, true> dag;
};

// Iterative Kosaraju. Component ids are in topological order; dag has no
// duplicate edges. Both SCC construction and condensation take O(V+E).
template<class Cost>
GraphSCC strongly_connected_components(const Graph<Cost, true>& graph) {
    int n = graph.size();
    std::vector<char> seen(n);
    std::vector<int> next(n), stack, finish; stack.reserve(n); finish.reserve(n);
    for (int root = 0; root < n; ++root) if (!seen[root]) {
        seen[root] = true; stack.push_back(root);
        while (!stack.empty()) {
            int v = stack.back();
            if (next[v] == graph.degree(v)) { finish.push_back(v); stack.pop_back(); continue; }
            int to = graph[v][next[v]++].to;
            if (!seen[to]) { seen[to] = true; stack.push_back(to); }
        }
    }
    std::vector<std::vector<int>> reverse(n);
    auto indegree = graph.indegree();
    for (int v = 0; v < n; ++v) reverse[v].reserve(indegree[v]);
    for (const auto& e : graph.edges) reverse[e.to].push_back(e.from);
    GraphSCC result;
    result.component.assign(n, -1);
    for (auto it = finish.rbegin(); it != finish.rend(); ++it) if (result.component[*it] == -1) {
        int id = result.size(); result.groups.push_back({});
        result.component[*it] = id; stack.push_back(*it);
        while (!stack.empty()) {
            int v = stack.back(); stack.pop_back(); result.groups.back().push_back(v);
            for (int to : reverse[v]) if (result.component[to] == -1) {
                result.component[to] = id; stack.push_back(to);
            }
        }
    }
    result.dag = Graph<int, true>(result.size());
    std::vector<int> stamp(result.size(), -1);
    for (int id = 0; id < result.size(); ++id)
        for (int v : result.groups[id]) for (const auto& e : graph[v]) {
            int to = result.component[e.to];
            if (to != id && stamp[to] != id) { stamp[to] = id; result.dag.add_edge(id, to); }
        }
    return result;
}

// The returned path is closed: vertices.front() == vertices.back().
// Parent edge ids, rather than parent vertices, handle parallel edges.
template<class Cost, bool Directed>
std::optional<GraphPath> find_cycle(const Graph<Cost, Directed>& graph) {
    int n = graph.size();
    std::vector<char> state(n);
    std::vector<int> parent(n, -1), parent_edge(n, -1), next(n), stack;
    stack.reserve(n);
    for (int root = 0; root < n; ++root) if (!state[root]) {
        state[root] = 1; stack.push_back(root);
        while (!stack.empty()) {
            int v = stack.back();
            if (next[v] == graph.degree(v)) { state[v] = 2; stack.pop_back(); continue; }
            const auto& e = graph[v][next[v]++];
            if constexpr (!Directed) if (e.id == parent_edge[v]) continue;
            if (!state[e.to]) {
                parent[e.to] = v; parent_edge[e.to] = e.id;
                state[e.to] = 1; stack.push_back(e.to);
            } else if (state[e.to] == 1) {
                GraphPath cycle;
                for (int u = v; u != e.to; u = parent[u]) {
                    cycle.vertices.push_back(u); cycle.edge_ids.push_back(parent_edge[u]);
                }
                cycle.vertices.push_back(e.to);
                std::reverse(cycle.vertices.begin(), cycle.vertices.end());
                std::reverse(cycle.edge_ids.begin(), cycle.edge_ids.end());
                cycle.vertices.push_back(e.to); cycle.edge_ids.push_back(e.id);
                return cycle;
            }
        }
    }
    return std::nullopt;
}
