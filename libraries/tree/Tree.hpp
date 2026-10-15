// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/tree/Tree.hpp

#pragma once
#include "libraries/graph/Graph.hpp"

struct TreePathSegment {
    int left, right;
    bool reversed; // Read vertex[right-1], ..., vertex[left] when true.
};

template<class Cost>
struct TreeDiameter {
    Cost length;
    GraphPath path;
};

template<class Value = long long>
class TreeAddSum;

// An owned undirected forest with iterative heavy-light preprocessing.
// Add edges, then build(). Paths/queries require an up-to-date build.
template<class Cost = long long>
struct Tree {
    Graph<Cost, false> graph;
    int n, root = 0;
    std::vector<int> parent, parent_edge, depth, component, roots, order;
    std::vector<int> id, out, vertex, subsize, heavy_child, head;
    std::vector<Cost> root_distance;

    explicit Tree(int n_ = 0) : graph(n_), n(n_) {}
    explicit Tree(const Graph<Cost, false>& source, int root_ = 0)
        : graph(source), n(source.size()) { build(root_); }

    int size() const { return n; }
    int add_edge(int from, int to, Cost cost = Cost(1)) {
        ready = false;
        return graph.add_edge(from, to, cost);
    }
    template<bool Weighted = false>
    void read_edges(int count, int index_offset, std::istream& input = std::cin) {
        read_edges<Weighted>(count, input, index_offset);
    }
    template<bool Weighted = false>
    void read_edges(int count = -1, std::istream& input = std::cin, int index_offset = 1) {
        if (count == -1) count = std::max(0, n - 1);
        ready = false;
        graph.template read_edges<Weighted>(count, input, index_offset);
    }

    // Vertex values, independent of edge costs. Copies the current rooted layout.
    template<class Value = long long>
    TreeAddSum<Value> make_add_sum() const;
    template<class Value>
    TreeAddSum<Value> make_add_sum(const std::vector<Value>& initial_values) const;

    void build(int root_ = 0) {
        ready = false; n = graph.size(); root = root_;
        assert(n == 0 || (0 <= root && root < n));
        parent.assign(n, -1); parent_edge.assign(n, -1); depth.assign(n, 0);
        root_distance.assign(n, Cost(0)); component.assign(n, -1);
        roots.clear(); order.clear(); order.reserve(n);
        std::vector<int> stack; stack.reserve(n);
        auto visit_component = [&](int start) {
            int cid = (int)roots.size(); roots.push_back(start);
            component[start] = cid; stack.push_back(start);
            while (!stack.empty()) {
                int v = stack.back(); stack.pop_back(); order.push_back(v);
                const auto& row = graph[v];
                for (auto it = row.rbegin(); it != row.rend(); ++it) {
                    const auto& e = *it;
                    if (e.id == parent_edge[v]) continue;
                    if (component[e.to] != -1) throw std::invalid_argument("Tree requires an undirected forest");
                    component[e.to] = cid; parent[e.to] = v; parent_edge[e.to] = e.id;
                    depth[e.to] = depth[v] + 1; root_distance[e.to] = root_distance[v] + e.cost;
                    stack.push_back(e.to);
                }
            }
        };
        if (n) visit_component(root);
        for (int v = 0; v < n; ++v) if (component[v] == -1) visit_component(v);

        subsize.assign(n, 1); heavy_child.assign(n, -1);
        for (auto it = order.rbegin(); it != order.rend(); ++it) {
            int v = *it, maximum = 0;
            for (const auto& e : graph[v]) if (parent[e.to] == v) {
                subsize[v] += subsize[e.to];
                if (maximum < subsize[e.to]) { maximum = subsize[e.to]; heavy_child[v] = e.to; }
            }
        }
        id.resize(n); out.resize(n); vertex.resize(n); head.resize(n);
        std::vector<std::pair<int, int>> pending; pending.reserve(n);
        for (auto it = roots.rbegin(); it != roots.rend(); ++it) pending.push_back({*it, *it});
        int timer = 0;
        while (!pending.empty()) {
            auto [v, chain_head] = pending.back(); pending.pop_back();
            for (; v != -1; v = heavy_child[v]) {
                head[v] = chain_head; id[v] = timer; vertex[timer++] = v;
                const auto& row = graph[v];
                for (auto it = row.rbegin(); it != row.rend(); ++it)
                    if (parent[it->to] == v && it->to != heavy_child[v]) pending.push_back({it->to, it->to});
            }
        }
        for (int v = 0; v < n; ++v) out[v] = id[v] + subsize[v];
        built_edge_count = graph.edge_count(); ready = true;
    }

    bool connected(int u, int v) const {
        check_vertex(u); check_vertex(v);
        return component[u] == component[v];
    }
    int lca(int u, int v) const {
        if (!connected(u, v)) return -1;
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) u = parent[head[u]];
            else v = parent[head[v]];
        }
        return depth[u] < depth[v] ? u : v;
    }
    // Edge count, independent of costs. -1 if components differ.
    int distance(int u, int v) const {
        int ancestor = lca(u, v);
        return ancestor == -1 ? -1 : depth[u] + depth[v] - 2 * depth[ancestor];
    }
    std::optional<Cost> weighted_distance(int u, int v) const {
        int ancestor = lca(u, v);
        if (ancestor == -1) return std::nullopt;
        return (root_distance[u] - root_distance[ancestor]) + (root_distance[v] - root_distance[ancestor]);
    }
    // Ancestor at absolute depth d (compatible with HLD/VirtualTree).
    int level_ancestor(int v, int d) const {
        check_vertex(v);
        if (d < 0 || depth[v] < d) return -1;
        while (depth[head[v]] > d) v = parent[head[v]];
        return vertex[id[v] - (depth[v] - d)];
    }
    // Ancestor k edges above v.
    int ancestor(int v, int k) const {
        check_vertex(v);
        if (k < 0 || depth[v] < k) return -1;
        return level_ancestor(v, depth[v] - k);
    }
    int jump(int u, int v, int k) const {
        int a = lca(u, v);
        if (a == -1 || k < 0) return -1;
        int up = depth[u] - depth[a], down = depth[v] - depth[a];
        if (k > up + down) return -1;
        return k <= up ? ancestor(u, k) : ancestor(v, up + down - k);
    }
    bool in_subtree(int ancestor_vertex, int v) const {
        check_vertex(ancestor_vertex); check_vertex(v);
        return id[ancestor_vertex] <= id[v] && id[v] < out[ancestor_vertex];
    }
    std::pair<int, int> subtree_range(int v) const { check_vertex(v); return {id[v], out[v]}; }

    // Segments are ordered from u to v. In edge mode, an edge is represented
    // by its deeper endpoint's position, and the LCA position is excluded.
    std::optional<std::vector<TreePathSegment>> path_segments(int u, int v, bool vertex_query = true) const {
        if (!connected(u, v)) return std::nullopt;
        std::vector<TreePathSegment> left, right;
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                left.push_back({id[head[u]], id[u] + 1, true}); u = parent[head[u]];
            } else {
                right.push_back({id[head[v]], id[v] + 1, false}); v = parent[head[v]];
            }
        }
        int l = std::min(id[u], id[v]) + (vertex_query ? 0 : 1), r = std::max(id[u], id[v]) + 1;
        if (l < r) left.push_back({l, r, depth[u] > depth[v]});
        left.insert(left.end(), right.rbegin(), right.rend());
        return left;
    }

    std::optional<GraphPath> path(int u, int v) const {
        int a = lca(u, v);
        if (a == -1) return std::nullopt;
        GraphPath result;
        std::vector<int> tail, tail_edges;
        for (int current = u; current != a; current = parent[current]) {
            result.vertices.push_back(current); result.edge_ids.push_back(parent_edge[current]);
        }
        result.vertices.push_back(a);
        for (int current = v; current != a; current = parent[current]) {
            tail.push_back(current); tail_edges.push_back(parent_edge[current]);
        }
        result.vertices.insert(result.vertices.end(), tail.rbegin(), tail.rend());
        result.edge_ids.insert(result.edge_ids.end(), tail_edges.rbegin(), tail_edges.rend());
        return result;
    }

    std::vector<int> centroids() const {
        check_ready();
        std::vector<int> result;
        for (int v = 0; v < n; ++v) {
            int total = subsize[roots[component[v]]], largest = total - subsize[v];
            if (heavy_child[v] != -1) largest = std::max(largest, subsize[heavy_child[v]]);
            if (largest <= total / 2) result.push_back(v);
        }
        return result;
    }

    // Longest weighted simple path over all components. A zero-edge path
    // is allowed, so negative weights are supported and length is >= 0.
    std::optional<TreeDiameter<Cost>> diameter() const {
        check_ready();
        if (!n) return std::nullopt;
        std::vector<Cost> down(n, Cost(0));
        std::vector<int> endpoint(n); std::iota(endpoint.begin(), endpoint.end(), 0);
        Cost best = Cost(0); int from = roots.front(), to = from;
        for (auto it = order.rbegin(); it != order.rend(); ++it) {
            int v = *it;
            for (const auto& e : graph[v]) if (parent[e.to] == v) {
                Cost branch = e.cost + down[e.to];
                if (!(Cost(0) < branch)) continue;
                Cost candidate = down[v] + branch;
                if (best < candidate) { best = candidate; from = endpoint[v]; to = endpoint[e.to]; }
                if (down[v] < branch) { down[v] = branch; endpoint[v] = endpoint[e.to]; }
            }
        }
        return TreeDiameter<Cost>{best, *path(from, to)};
    }

private:
    template<class> friend class TreeAddSum;
    bool ready = false;
    int built_edge_count = -1;
    void check_ready() const { assert(ready && n == graph.size() && built_edge_count == graph.edge_count()); }
    void check_vertex(int v) const { check_ready(); assert(0 <= v && v < n); (void)v; }
};

// A snapshot of a built Tree's HLD layout with two Fenwick trees.
// Subtree add/sum: O(log n), path add/sum: O(log^2 n), build: O(n).
// Rebuilding/destroying the source Tree does not affect this object.
template<class Value>
class TreeAddSum {
public:
    template<class Cost>
    explicit TreeAddSum(const Tree<Cost>& tree)
        : TreeAddSum(tree, std::vector<Value>(tree.size(), Value(0))) {}

    template<class Cost>
    TreeAddSum(const Tree<Cost>& tree, const std::vector<Value>& initial_values)
        : n_(tree.size()) {
        tree.check_ready();
        assert((int)initial_values.size() == n_);
        id_ = tree.id; out_ = tree.out; head_ = tree.head;
        parent_ = tree.parent; depth_ = tree.depth; component_ = tree.component;
        difference_.assign(n_ + 1, Value(0));
        weighted_.assign(n_ + 1, Value(0));
        Value previous = Value(0);
        for (int i = 0; i < n_; ++i) {
            Value current = initial_values[tree.vertex[i]];
            difference_[i + 1] = current - previous;
            weighted_[i + 1] = difference_[i + 1] * Value(i);
            previous = current;
        }
        for (int i = 1; i <= n_; ++i) {
            int next = i + (i & -i);
            if (next <= n_) {
                difference_[next] += difference_[i];
                weighted_[next] += weighted_[i];
            }
        }
    }

    int size() const { return n_; }

    Value get(int v) const {
        check_vertex(v);
        Value result = Value(0);
        for (int i = id_[v] + 1; i > 0; i -= i & -i) result += difference_[i];
        return result;
    }
    void set(int v, Value value) { add_vertex(v, value - get(v)); }
    void add_vertex(int v, Value delta) {
        check_vertex(v);
        add_range(id_[v], id_[v] + 1, delta);
    }
    void add_subtree(int v, Value delta) {
        check_vertex(v);
        add_range(id_[v], out_[v], delta);
    }
    Value sum_subtree(int v) const {
        check_vertex(v);
        return sum_range(id_[v], out_[v]);
    }
    Value sum_all() const { return prefix_sum(n_); }

    // Both endpoints are included. A cross-component update changes nothing.
    bool add_path(int u, int v, Value delta) {
        return for_each_path_range(u, v, [&](int l, int r) { add_range(l, r, delta); });
    }
    std::optional<Value> sum_path(int u, int v) const {
        Value result = Value(0);
        if (!for_each_path_range(u, v, [&](int l, int r) { result += sum_range(l, r); }))
            return std::nullopt;
        return result;
    }

private:
    int n_;
    std::vector<int> id_, out_, head_, parent_, depth_, component_;
    std::vector<Value> difference_, weighted_;

    void check_vertex(int v) const { assert(0 <= v && v < n_); (void)v; }
    void add_boundary(int position, Value delta) {
        if (position == n_) return;
        Value scaled = delta * Value(position);
        for (int i = position + 1; i <= n_; i += i & -i) {
            difference_[i] += delta;
            weighted_[i] += scaled;
        }
    }
    void add_range(int l, int r, Value delta) {
        add_boundary(l, delta);
        add_boundary(r, Value(0) - delta);
    }
    Value prefix_sum(int r) const {
        Value difference = Value(0), weighted = Value(0);
        for (int i = r; i > 0; i -= i & -i) {
            difference += difference_[i];
            weighted += weighted_[i];
        }
        return difference * Value(r) - weighted;
    }
    Value sum_range(int l, int r) const { return prefix_sum(r) - prefix_sum(l); }

    template<class Callback>
    bool for_each_path_range(int u, int v, Callback callback) const {
        check_vertex(u); check_vertex(v);
        if (component_[u] != component_[v]) return false;
        while (head_[u] != head_[v]) {
            if (depth_[head_[u]] < depth_[head_[v]]) std::swap(u, v);
            callback(id_[head_[u]], id_[u] + 1);
            u = parent_[head_[u]];
        }
        callback(std::min(id_[u], id_[v]), std::max(id_[u], id_[v]) + 1);
        return true;
    }
};

template<class Cost>
template<class Value>
TreeAddSum<Value> Tree<Cost>::make_add_sum() const {
    return TreeAddSum<Value>(*this);
}

template<class Cost>
template<class Value>
TreeAddSum<Value> Tree<Cost>::make_add_sum(const std::vector<Value>& initial_values) const {
    return TreeAddSum<Value>(*this, initial_values);
}
