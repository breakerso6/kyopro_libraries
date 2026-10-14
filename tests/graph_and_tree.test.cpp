#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include <bits/stdc++.h>
#include "libraries/graph/GraphAlgorithms.hpp"
#include "libraries/tree/Tree.hpp"
#include "libraries/tree/HLD.hpp"
#include "libraries/tree/VirtualTree.hpp"
#include "libraries/graph/Dijkstra.hpp"
#include "libraries/graph/LowLink.hpp"

using namespace std;
constexpr long long INF = 1000000000000LL;

template<class Cost, bool Directed>
Cost validate_path(const Graph<Cost, Directed>& graph, const GraphPath& path) {
    assert(!path.vertices.empty() && path.edge_ids.size() + 1 == path.vertices.size());
    Cost total = 0;
    for (size_t i = 0; i < path.edge_ids.size(); ++i) {
        int id = path.edge_ids[i], u = path.vertices[i], v = path.vertices[i + 1];
        assert(0 <= id && id < graph.edge_count());
        const auto& e = graph.edges[id];
        bool valid = e.from == u && e.to == v;
        if constexpr (!Directed) valid = valid || (e.from == v && e.to == u);
        assert(valid); total += e.cost;
    }
    return total;
}

template<class Cost, bool Directed>
vector<vector<long long>> floyd(const Graph<Cost, Directed>& graph, bool ignore_cost = false) {
    int n = graph.size();
    vector<vector<long long>> distance(n, vector<long long>(n, INF));
    for (int v = 0; v < n; ++v) distance[v][v] = 0;
    for (int v = 0; v < n; ++v) for (const auto& e : graph[v])
        distance[v][e.to] = min(distance[v][e.to], ignore_cost ? 1LL : (long long)e.cost);
    for (int k = 0; k < n; ++k) for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j)
        if (distance[i][k] != INF && distance[k][j] != INF)
            distance[i][j] = min(distance[i][j], distance[i][k] + distance[k][j]);
    return distance;
}

template<class Cost, bool Directed, class Distance>
void validate_search(const Graph<Cost, Directed>& graph, const GraphSearchResult<Distance>& result,
                     const vector<int>& sources, const vector<vector<long long>>& expected, bool ignore_cost) {
    vector<char> seen(graph.size());
    for (int v : result.order) { assert(!seen[v] && result.reached[v]); seen[v] = true; }
    for (int v = 0; v < graph.size(); ++v) {
        long long best = INF;
        for (int source : sources) best = min(best, expected[source][v]);
        assert(bool(result.reached[v]) == (best != INF));
        assert(bool(seen[v]) == bool(result.reached[v]));
        auto path = result.path_to(v);
        assert(bool(path) == bool(result.reached[v]));
        if (!path) continue;
        assert(result.distance[v] == best && path->vertices.back() == v);
        assert(find(sources.begin(), sources.end(), path->vertices.front()) != sources.end());
        auto cost = validate_path(graph, *path);
        assert((ignore_cost ? (long long)path->edge_ids.size() : (long long)cost) == best);
    }
}

template<bool Directed>
void test_searches() {
    mt19937 rng(Directed ? 11223 : 99887);
    for (int trial = 0; trial < 120; ++trial) {
        int n = rng() % 12;
        Graph<long long, Directed> graph(n), binary(n);
        int edges = n ? rng() % 50 : 0;
        for (int i = 0; i < edges; ++i) {
            int u = rng() % n, v = rng() % n;
            assert(graph.add_edge(u, v, rng() % 20) == i);
            binary.add_edge(u, v, rng() % 2);
        }
        vector<int> sources;
        if (n) sources = {(int)(rng() % n), (int)(rng() % n)};
        validate_search(graph, breadth_first_search(graph, sources), sources, floyd(graph, true), true);
        validate_search(graph, dijkstra(graph, sources, INF), sources, floyd(graph), false);
        validate_search(binary, zero_one_bfs(binary, sources), sources, floyd(binary), false);
        if (n) {
            validate_search(graph, breadth_first_search(graph, sources[0]), {sources[0]}, floyd(graph, true), true);
            validate_search(graph, dijkstra(graph, sources[0], INF), {sources[0]}, floyd(graph), false);
            validate_search(binary, zero_one_bfs(binary, sources[0]), {sources[0]}, floyd(binary), false);
        }
        auto reverse = graph.reversed();
        assert(reverse.edge_count() == graph.edge_count());
        for (int id = 0; id < edges; ++id) {
            assert(reverse.edges[id].from == graph.edges[id].to);
            assert(reverse.edges[id].to == graph.edges[id].from);
            assert(reverse.edges[id].cost == graph.edges[id].cost && reverse.edges[id].id == id);
        }
    }
}

template<bool Weighted>
static void test_edge_input() {
    for (int offset : {0, 1}) {
        ostringstream text;
        text << offset << ' ' << offset + 1;
        if constexpr (Weighted) text << " 7";
        text << '\n' << offset + 1 << ' ' << offset + 2;
        if constexpr (Weighted) text << " -2";
        text << '\n';
        Graph<long long, true> graph(3);
        istringstream graph_input(text.str());
        graph.template read_edges<Weighted>(2, offset, graph_input);
        assert(graph.edges[0].from == 0 && graph.edges[0].to == 1);
        assert(graph.edges[1].from == 1 && graph.edges[1].to == 2);
        assert(graph.edges[0].cost == (Weighted ? 7 : 1));
        assert(graph.edges[1].cost == (Weighted ? -2 : 1));
        assert(graph.edges[1].id == 1 && graph.degree(2) == 0);

        Tree<> tree(3);
        tree.build();
        istringstream tree_input(text.str());
        tree.template read_edges<Weighted>(2, offset, tree_input);
        tree.build(2);
        assert(tree.lca(0, 1) == 1);
        assert(tree.weighted_distance(0, 2) == optional<long long>(Weighted ? 5 : 2));

        // Exercise the contest-facing overloads that default to std::cin.
        istringstream contest_input(text.str() + text.str());
        auto* previous = cin.rdbuf(contest_input.rdbuf());
        Graph<> from_cin(3);
        Tree<> tree_from_cin(3);
        if (offset == 1) {
            from_cin.template read_edges<Weighted>(2);
            tree_from_cin.template read_edges<Weighted>(2);
        } else {
            from_cin.template read_edges<Weighted>(2, 0);
            tree_from_cin.template read_edges<Weighted>(2, 0);
        }
        cin.rdbuf(previous);
        tree_from_cin.build();
        assert(from_cin.edges[1].to == 2 && from_cin.edges[1].cost == (Weighted ? -2 : 1));
        assert(tree_from_cin.weighted_distance(0, 2) == optional<long long>(Weighted ? 5 : 2));
    }
}

static void test_storage_and_limits() {
    Graph<> graph(3);
    istringstream unweighted("1 2\n2 3\n2 2\n");
    graph.read_edges(3, unweighted);
    assert(graph.degree(1) == 4 && graph.indegree() == vector<int>({1, 4, 1}));
    auto list = graph.unweighted_adjacency<long long>();
    assert(list[1] == vector<long long>({0, 2, 1, 1}));
    Graph<long long, true> weighted(3);
    istringstream input("0 1 7\n1 2 9\n");
    weighted.read_edges<true>(2, input, 0);
    assert(dijkstra(weighted, 0, INF).distance[2] == 16);
    Graph<long long, true> bulk(3, vector<tuple<int, int, long long>>{{0, 1, 7}, {1, 2, 9}});
    assert(dijkstra(bulk, 0, INF).distance == dijkstra(weighted, 0, INF).distance);
    bool failed = false;
    try { istringstream bad("0"); weighted.read_edges(1, bad, 0); }
    catch (const runtime_error&) { failed = true; }
    assert(failed);
    Graph<long long, true> large(4);
    large.add_edge(0, 1, LLONG_MAX); large.add_edge(1, 2, 1);
    large.add_edge(0, 3, LLONG_MAX - 2); large.add_edge(3, 2, 2);
    auto result = dijkstra(large, 0, LLONG_MAX);
    assert(result.reached[1] && result.distance[1] == LLONG_MAX);
    assert(result.reached[2] && result.distance[2] == LLONG_MAX);
    assert(result.path_to(2)->edge_ids == vector<int>({2, 3}));
    auto bounded = dijkstra(large, 0, 10LL);
    assert(bounded.reached[0] && !bounded.reached[1] && !bounded.reached[2] && !bounded.reached[3]);
    Graph<uint64_t, true> unsigned_graph(3);
    unsigned_graph.add_edge(0, 1, UINT64_MAX); unsigned_graph.add_edge(1, 2, 1);
    auto unsigned_result = dijkstra(unsigned_graph, 0, UINT64_MAX);
    assert(unsigned_result.reached[1] && !unsigned_result.reached[2]);
    Graph<double, true> fractional(3);
    fractional.add_edge(0, 1, 0.25); fractional.add_edge(1, 2, 0.5);
    assert(dijkstra(fractional, 0, 100.0).distance[2] == 0.75);
    Graph<> zero(2); zero.add_edge(0, 1, 0);
    auto zero_result = dijkstra(zero, 0, 0LL);
    assert(zero_result.reached[1] && zero_result.distance[1] == 0);
    vector<int> no_sources;
    assert(breadth_first_search(zero, no_sources).order.empty());
    assert(zero_one_bfs(zero, no_sources).order.empty());
    assert(!dijkstra(zero, no_sources, INF).path_to(0));
}

static void test_directed_structure() {
    mt19937 rng(121212);
    for (int trial = 0; trial < 300; ++trial) {
        int n = rng() % 10;
        Graph<int, true> graph(n);
        for (int i = 0, edges = n ? rng() % 40 : 0; i < edges; ++i) graph.add_edge(rng() % n, rng() % n);
        auto distance = floyd(graph, true);
        bool cyclic = false;
        for (const auto& e : graph.edges) cyclic = cyclic || distance[e.to][e.from] != INF;
        auto cycle = find_cycle(graph);
        assert(bool(cycle) == cyclic);
        if (cycle) { validate_path(graph, *cycle); assert(cycle->vertices.front() == cycle->vertices.back()); }
        auto order = topological_sort(graph);
        assert(bool(order) == !cyclic);
        if (order) {
            vector<int> id(n, -1);
            for (int i = 0; i < n; ++i) { assert(id[(*order)[i]] == -1); id[(*order)[i]] = i; }
            for (const auto& e : graph.edges) assert(id[e.from] < id[e.to]);
        }
        auto scc = strongly_connected_components(graph);
        vector<int> seen(n);
        for (int id = 0; id < scc.size(); ++id) for (int v : scc.groups[id]) { assert(scc.component[v] == id); ++seen[v]; }
        for (int v = 0; v < n; ++v) assert(seen[v] == 1);
        for (int u = 0; u < n; ++u) for (int v = 0; v < n; ++v)
            assert((scc.component[u] == scc.component[v]) == (distance[u][v] != INF && distance[v][u] != INF));
        set<pair<int, int>> expected, actual;
        for (const auto& e : graph.edges) if (scc.component[e.from] != scc.component[e.to]) {
            assert(scc.component[e.from] < scc.component[e.to]);
            expected.insert({scc.component[e.from], scc.component[e.to]});
        }
        for (const auto& e : scc.dag.edges) { assert(actual.insert({e.from, e.to}).second); }
        assert(actual == expected && topological_sort(scc.dag));
    }
}

static void test_undirected_structure() {
    mt19937 rng(131313);
    for (int trial = 0; trial < 250; ++trial) {
        int n = rng() % 9;
        Graph<int> graph(n);
        for (int i = 0, edges = n ? rng() % 25 : 0; i < edges; ++i) graph.add_edge(rng() % n, rng() % n);
        auto distance = floyd(graph, true);
        auto cc = connected_components(graph);
        for (int u = 0; u < n; ++u) for (int v = 0; v < n; ++v)
            assert((cc.component[u] == cc.component[v]) == (distance[u][v] != INF));
        bool cyclic = graph.edge_count() > n - cc.size();
        auto cycle = find_cycle(graph);
        assert(bool(cycle) == cyclic);
        if (cycle) {
            validate_path(graph, *cycle); assert(cycle->vertices.front() == cycle->vertices.back());
            set<int> ids(cycle->edge_ids.begin(), cycle->edge_ids.end());
            assert(ids.size() == cycle->edge_ids.size());
        }
        bool bipartite = false;
        for (int mask = 0; mask < (1 << n); ++mask) {
            bool valid = true;
            for (const auto& e : graph.edges) if (((mask >> e.from) & 1) == ((mask >> e.to) & 1)) { valid = false; break; }
            bipartite = bipartite || valid;
        }
        auto color = bipartite_coloring(graph);
        assert(bool(color) == bipartite);
        if (color) for (const auto& e : graph.edges) assert((*color)[e.from] != (*color)[e.to]);
    }
}

static void test_tree() {
    mt19937 rng(141414);
    for (int trial = 0; trial < 120; ++trial) {
        int n = 1 + rng() % 40;
        Tree<long long> tree(n);
        for (int v = 1; v < n; ++v) if (rng() % 4)
            tree.add_edge(v, rng() % v, (int)(rng() % 21) - 10);
        tree.build(rng() % n);
        vector<int> seen(n);
        for (int i = 0; i < n; ++i) { assert(tree.id[tree.vertex[i]] == i); ++seen[tree.vertex[i]]; }
        for (int v = 0; v < n; ++v) {
            assert(seen[v] == 1);
            int descendants = 0;
            for (int u = 0; u < n; ++u) {
                bool is_descendant = false;
                for (int current = u; current != -1; current = tree.parent[current]) is_descendant |= current == v;
                assert(tree.in_subtree(v, u) == is_descendant); descendants += is_descendant;
            }
            assert(tree.subsize[v] == descendants);
            for (int k = -1; k <= tree.depth[v] + 1; ++k) {
                int expected = v;
                if (k < 0) expected = -1;
                else for (int step = 0; step < k && expected != -1; ++step) expected = tree.parent[expected];
                assert(tree.ancestor(v, k) == expected);
            }
        }
        long long longest = 0;
        for (int u = 0; u < n; ++u) {
            // The unique path is independent of the chosen root/HL ordering.
            auto traversal = breadth_first_search(tree.graph, u);
            for (int v = 0; v < n; ++v) {
                auto path = tree.path(u, v), expected = traversal.path_to(v);
                assert(bool(path) == bool(expected));
                assert(tree.distance(u, v) == traversal.distance[v]);
                if (!path) {
                    assert(tree.lca(u, v) == -1 && !tree.weighted_distance(u, v) && !tree.path_segments(u, v));
                    continue;
                }
                assert(path->vertices == expected->vertices && path->edge_ids == expected->edge_ids);
                long long cost = validate_path(tree.graph, *path);
                assert(tree.weighted_distance(u, v) == optional<long long>(cost)); longest = max(longest, cost);
                int a = u, b = v;
                while (tree.depth[a] > tree.depth[b]) a = tree.parent[a];
                while (tree.depth[b] > tree.depth[a]) b = tree.parent[b];
                while (a != b) a = tree.parent[a], b = tree.parent[b];
                assert(tree.lca(u, v) == a);
                for (int k = -1; k <= (int)path->vertices.size(); ++k)
                    assert(tree.jump(u, v, k) == (k < 0 || k >= (int)path->vertices.size() ? -1 : path->vertices[k]));
                vector<int> vertices, edges;
                auto vertex_segments = tree.path_segments(u, v);
                auto edge_segments = tree.path_segments(u, v, false);
                for (auto segment : *vertex_segments) {
                    if (segment.reversed) for (int i = segment.right - 1; i >= segment.left; --i) vertices.push_back(tree.vertex[i]);
                    else for (int i = segment.left; i < segment.right; ++i) vertices.push_back(tree.vertex[i]);
                }
                for (auto segment : *edge_segments) {
                    if (segment.reversed) for (int i = segment.right - 1; i >= segment.left; --i) edges.push_back(tree.parent_edge[tree.vertex[i]]);
                    else for (int i = segment.left; i < segment.right; ++i) edges.push_back(tree.parent_edge[tree.vertex[i]]);
                }
                assert(vertices == path->vertices && edges == path->edge_ids);
            }
        }
        auto diameter = tree.diameter();
        assert(diameter && diameter->length == longest && validate_path(tree.graph, diameter->path) == longest);
        auto centroids = tree.centroids();
        for (int removed = 0; removed < n; ++removed) {
            auto copy = tree.graph.unweighted_adjacency();
            vector<char> visited(n); visited[removed] = true;
            int largest = 0, total = tree.subsize[tree.roots[tree.component[removed]]];
            for (int start = 0; start < n; ++start) if (!visited[start] && tree.connected(start, removed)) {
                vector<int> queue{start}; visited[start] = true;
                for (size_t i = 0; i < queue.size(); ++i) for (int to : copy[queue[i]]) if (!visited[to]) {
                    visited[to] = true; queue.push_back(to);
                }
                largest = max(largest, (int)queue.size());
            }
            assert((find(centroids.begin(), centroids.end(), removed) != centroids.end()) == (largest <= total / 2));
        }
        tree.build(0); // Re-rooting preserves distances and path witnesses.
        assert(tree.diameter()->length == longest);
    }
}

static void test_adapters_and_large_inputs() {
    Graph<> graph(5);
    graph.add_edge(0, 1, 3); graph.add_edge(1, 2, 4); graph.add_edge(1, 3, 5); graph.add_edge(3, 4, 6);
    Tree<> tree(graph);
    HLD hld(graph.unweighted_adjacency<long long>());
    for (int u = 0; u < 5; ++u) for (int v = 0; v < 5; ++v) {
        assert(tree.lca(u, v) == hld.lca(u, v)); assert(tree.distance(u, v) == hld.distance(u, v));
    }
    auto virtual_tree = VirtualTree::build(vector<int>{2, 4}, tree);
    assert(virtual_tree.vertices.size() == 3);
    Dijkstra<long long> old_shortest(5);
    LowLink old_lowlink(5);
    for (const auto& e : graph.edges) { old_shortest.add_edge(e.from, e.to, e.cost, false); old_lowlink.add_edge(e.from, e.to); }
    assert(dijkstra(graph, 0, INF).distance == old_shortest.shortest_path(0, INF));
    old_lowlink.build(); assert(old_lowlink.bridges.size() == 4);
    Tree<> empty; empty.build(); assert(empty.centroids().empty() && !empty.diameter());
    Graph<> original(2); original.add_edge(0, 1, 4);
    Tree<> owned(original); original.add_edge(0, 1, 100);
    assert(owned.graph.edge_count() == 1 && owned.weighted_distance(0, 1) == optional<long long>(4));
    Tree<> parsed(3); istringstream tree_input("1 2 4\n2 3 -2\n");
    parsed.read_edges<true>(2, tree_input); parsed.build();
    assert(parsed.weighted_distance(0, 2) == optional<long long>(2) && parsed.diameter()->length == 4);
    Tree<> changed(3); changed.add_edge(0, 1, 4); changed.build();
    assert(!changed.connected(0, 2));
    changed.add_edge(1, 2, 5); changed.build(2);
    assert(changed.connected(0, 2) && changed.weighted_distance(0, 2) == optional<long long>(9));
    Tree<> invalid(2); invalid.add_edge(0, 1); invalid.add_edge(0, 1);
    bool rejected = false;
    try { invalid.build(); } catch (const invalid_argument&) { rejected = true; }
    assert(rejected);
    Tree<> self_loop(1); self_loop.add_edge(0, 0); rejected = false;
    try { self_loop.build(); } catch (const invalid_argument&) { rejected = true; }
    assert(rejected);
    Tree<> triangle(3); triangle.add_edge(0, 1); triangle.add_edge(1, 2); triangle.add_edge(2, 0); rejected = false;
    try { triangle.build(); } catch (const invalid_argument&) { rejected = true; }
    assert(rejected);
    const int n = 200000;
    Tree<> chain(n);
    Graph<int, true> directed(n);
    for (int v = 1; v < n; ++v) { chain.add_edge(v - 1, v); directed.add_edge(v - 1, v); }
    chain.build(n - 1);
    assert(chain.lca(0, n / 2) == n / 2 && chain.distance(0, n - 1) == n - 1);
    assert(chain.jump(0, n - 1, n - 1) == n - 1 && chain.diameter()->length == n - 1);
    assert(!find_cycle(directed) && topological_sort(directed)->size() == n);
    assert(strongly_connected_components(directed).size() == n);
    directed.add_edge(n - 1, 0);
    auto cycle = find_cycle(directed);
    assert(cycle && cycle->edge_ids.size() == n);
    assert(strongly_connected_components(directed).size() == 1);
}

int main() {
    test_edge_input<false>(); test_edge_input<true>();
    test_storage_and_limits(); test_searches<false>(); test_searches<true>();
    test_directed_structure(); test_undirected_structure(); test_tree(); test_adapters_and_large_inputs();
    cout << "Hello World\n";
}
