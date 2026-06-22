#include "libraries/tree/HLD_seg.hpp"

struct S {
    string value;
};

S op(S a, S b) {
    return {a.value + b.value};
}

S e() {
    return {""};
}

namespace {

struct NaiveTree {
    vector<vector<int>> graph;
    vector<int> parent;
    vector<int> depth;

    NaiveTree(const vector<vector<ll>>& g, int root) {
        int n = (int)g.size();
        graph.assign(n, {});
        parent.assign(n, -1);
        depth.assign(n, 0);
        for (int v = 0; v < n; v++) {
            for (ll to : g[v]) graph[v].push_back((int)to);
        }
        queue<int> que;
        que.push(root);
        while (!que.empty()) {
            int v = que.front();
            que.pop();
            for (int to : graph[v]) {
                if (to == parent[v]) continue;
                parent[to] = v;
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

    vector<int> path(int s, int t) const {
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
};

vector<vector<ll>> make_graph(int n, const vector<pair<int, int>>& edges) {
    vector<vector<ll>> g(n);
    for (auto [u, v] : edges) {
        g[u].push_back(v);
        g[v].push_back(u);
    }
    return g;
}

void check_equal(const string& actual, const string& expected) {
    if (actual != expected) {
        cerr << "actual:   " << actual << '\n';
        cerr << "expected: " << expected << '\n';
        assert(false);
    }
}

void check_tree(const vector<vector<ll>>& g, int root, int seed) {
    int n = (int)g.size();
    vector<S> initial_values(n);
    vector<char> values(n);
    for (int v = 0; v < n; v++) {
        values[v] = char('a' + (v % 26));
        initial_values[v] = {string(1, values[v])};
    }

    HLD_seg<S, op, e> hld(g, initial_values, root);
    NaiveTree naive(g, root);

    for (int v = 0; v < n; v++) {
        for (int d = 0; d <= n + 1; d++) {
            assert(hld.level_ancestor(v, d) == naive.level_ancestor(v, d));
        }
        check_equal(hld.get(v).value, string(1, values[v]));
    }

    auto expected_path_value = [&](int u, int v) {
        string expected;
        for (int x : naive.path(u, v)) expected.push_back(values[x]);
        return expected;
    };

    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            assert(hld.lca(u, v) == naive.lca(u, v));
            assert(hld.distance(u, v) == naive.distance(u, v));
            vector<int> path = naive.path(u, v);
            for (int i = 0; i < (int)path.size(); i++) {
                assert(hld.jump(u, v, i) == path[i]);
            }
            check_equal(hld.prod_path(u, v).value, expected_path_value(u, v));
        }
    }

    mt19937 rng(seed);
    for (int q = 0; q < 300; q++) {
        int type = uniform_int_distribution<int>(0, 1)(rng);
        int u = uniform_int_distribution<int>(0, n - 1)(rng);
        int v = uniform_int_distribution<int>(0, n - 1)(rng);
        char c = char('A' + uniform_int_distribution<int>(0, 25)(rng));

        if (type == 0) {
            hld.set(u, {string(1, c)});
            values[u] = c;
            check_equal(hld.get(u).value, string(1, c));
        } else {
            check_equal(hld.prod_path(u, v).value, expected_path_value(u, v));
            check_equal(hld.prod_path(v, u).value, expected_path_value(v, u));
        }
    }
}

void check_all_roots(const vector<vector<ll>>& g) {
    for (int root = 0; root < (int)g.size(); root++) check_tree(g, root, 2000 + root);
}

}  // namespace

int main() {
    check_all_roots(make_graph(1, {}));
    check_all_roots(make_graph(2, {{0, 1}}));
    check_all_roots(make_graph(8, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}}));
    check_all_roots(make_graph(8, {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}}));
    check_all_roots(make_graph(10, {{0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {2, 6}, {4, 7}, {4, 8}, {6, 9}}));

    mt19937 rng(987654321);
    for (int n = 1; n <= 35; n++) {
        for (int tc = 0; tc < 20; tc++) {
            vector<pair<int, int>> edges;
            for (int v = 1; v < n; v++) {
                int p = uniform_int_distribution<int>(0, v - 1)(rng);
                edges.push_back({p, v});
            }
            vector<vector<ll>> g = make_graph(n, edges);
            for (int rep = 0; rep < min(n, 3); rep++) {
                int root = uniform_int_distribution<int>(0, n - 1)(rng);
                check_tree(g, root, 2000000 + n * 1000 + tc * 10 + rep);
            }
        }
    }

    cout << "HLD_seg tests passed\n";
}
