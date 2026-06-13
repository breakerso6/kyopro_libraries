#include "../libraries/HLD_lseg_edge.hpp"

struct S {
    string value;
    int len;
};

struct F {
    char assign;
};

S op(S a, S b) {
    return {a.value + b.value, a.len + b.len};
}

S e() {
    return {"", 0};
}

S mapping(F f, S x) {
    if (f.assign == 0) return x;
    return {string(x.len, f.assign), x.len};
}

F composition(F f, F g) {
    return f.assign == 0 ? g : f;
}

F id() {
    return {0};
}

namespace {

struct NaiveTree {
    vector<vector<pair<int, int>>> graph;
    vector<int> parent;
    vector<int> parent_edge;
    vector<int> depth;

    NaiveTree(int n, const vector<pair<int, int>>& edges, int root) {
        graph.assign(n, {});
        parent.assign(n, -1);
        parent_edge.assign(n, -1);
        depth.assign(n, 0);
        for (int i = 0; i < (int)edges.size(); i++) {
            auto [u, v] = edges[i];
            graph[u].push_back({v, i});
            graph[v].push_back({u, i});
        }
        queue<int> que;
        que.push(root);
        while (!que.empty()) {
            int v = que.front();
            que.pop();
            for (auto [to, eid] : graph[v]) {
                if (to == parent[v]) continue;
                parent[to] = v;
                parent_edge[to] = eid;
                depth[to] = depth[v] + 1;
                que.push(to);
            }
        }
    }

    int level_ancestor(int v, int d) const {
        if (depth[v] < d) return -1;
        while (depth[v] > d) v = parent[v];
        return v;
    }

    int lca(int u, int v) const {
        while (depth[u] > depth[v]) u = parent[u];
        while (depth[v] > depth[u]) v = parent[v];
        while (u != v) {
            u = parent[u];
            v = parent[v];
        }
        return u;
    }

    int distance(int u, int v) const {
        int l = lca(u, v);
        return depth[u] + depth[v] - 2 * depth[l];
    }

    vector<int> vertex_path(int s, int t) const {
        int l = lca(s, t);
        vector<int> left;
        for (int v = s; v != l; v = parent[v]) left.push_back(v);
        left.push_back(l);

        vector<int> right;
        for (int v = t; v != l; v = parent[v]) right.push_back(v);
        reverse(right.begin(), right.end());
        left.insert(left.end(), right.begin(), right.end());
        return left;
    }

    vector<int> edge_path(int s, int t) const {
        int l = lca(s, t);
        vector<int> left;
        for (int v = s; v != l; v = parent[v]) left.push_back(parent_edge[v]);

        vector<int> right;
        for (int v = t; v != l; v = parent[v]) right.push_back(parent_edge[v]);
        reverse(right.begin(), right.end());
        left.insert(left.end(), right.begin(), right.end());
        return left;
    }
};

vector<edge<S>> make_edges(const vector<pair<int, int>>& edge_pairs, const vector<char>& values) {
    vector<edge<S>> edges;
    for (int i = 0; i < (int)edge_pairs.size(); i++) {
        auto [u, v] = edge_pairs[i];
        edges.push_back(edge<S>(u, v, {string(1, values[i]), 1}));
    }
    return edges;
}

void check_equal(const string& actual, const string& expected) {
    if (actual != expected) {
        cerr << "actual:   " << actual << '\n';
        cerr << "expected: " << expected << '\n';
        assert(false);
    }
}

void check_tree(int n, const vector<pair<int, int>>& edge_pairs, int root, int seed) {
    vector<char> values(max(0, n - 1));
    for (int i = 0; i < n - 1; i++) values[i] = char('a' + (i % 26));

    HLD_lseg_edge<S, op, e, F, mapping, composition, id> hld(n, make_edges(edge_pairs, values), root);
    NaiveTree naive(n, edge_pairs, root);

    for (int v = 0; v < n; v++) {
        for (int d = 0; d <= n + 1; d++) {
            assert(hld.level_ancestor(v, d) == naive.level_ancestor(v, d));
        }
    }
    for (int i = 0; i < n - 1; i++) {
        check_equal(hld.get(i).value, string(1, values[i]));
    }
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            assert(hld.lca(u, v) == naive.lca(u, v));
            assert(hld.distance(u, v) == naive.distance(u, v));
            vector<int> path = naive.vertex_path(u, v);
            for (int i = 0; i < (int)path.size(); i++) {
                assert(hld.jump(u, v, i) == path[i]);
            }
        }
    }

    auto expected_path_value = [&](int u, int v) {
        string expected;
        for (int eid : naive.edge_path(u, v)) expected.push_back(values[eid]);
        return expected;
    };

    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            check_equal(hld.prod_path(u, v).value, expected_path_value(u, v));
        }
    }

    mt19937 rng(seed);
    for (int q = 0; q < 300; q++) {
        int type = uniform_int_distribution<int>(0, 2)(rng);
        int u = uniform_int_distribution<int>(0, n - 1)(rng);
        int v = uniform_int_distribution<int>(0, n - 1)(rng);
        char c = char('A' + uniform_int_distribution<int>(0, 25)(rng));

        if (type == 0 && n > 1) {
            int eid = uniform_int_distribution<int>(0, n - 2)(rng);
            hld.set(eid, {string(1, c), 1});
            values[eid] = c;
            check_equal(hld.get(eid).value, string(1, c));
        } else if (type == 1) {
            hld.apply_path(u, v, {c});
            for (int eid : naive.edge_path(u, v)) values[eid] = c;
        } else {
            check_equal(hld.prod_path(u, v).value, expected_path_value(u, v));
            check_equal(hld.prod_path(v, u).value, expected_path_value(v, u));
        }
    }
}

void check_all_roots(int n, const vector<pair<int, int>>& edge_pairs) {
    for (int root = 0; root < n; root++) check_tree(n, edge_pairs, root, 4000 + root);
}

}  // namespace

int main() {
    check_all_roots(1, {});
    check_all_roots(2, {{0, 1}});
    check_all_roots(8, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}});
    check_all_roots(8, {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}});
    check_all_roots(10, {{0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {2, 6}, {4, 7}, {4, 8}, {6, 9}});

    mt19937 rng(975318642);
    for (int n = 1; n <= 35; n++) {
        for (int tc = 0; tc < 20; tc++) {
            vector<pair<int, int>> edges;
            for (int v = 1; v < n; v++) {
                int p = uniform_int_distribution<int>(0, v - 1)(rng);
                edges.push_back({p, v});
            }
            for (int rep = 0; rep < min(n, 3); rep++) {
                int root = uniform_int_distribution<int>(0, n - 1)(rng);
                check_tree(n, edges, root, 4000000 + n * 1000 + tc * 10 + rep);
            }
        }
    }

    cout << "HLD_lseg_edge tests passed\n";
}
