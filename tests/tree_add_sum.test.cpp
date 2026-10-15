#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include "libraries/tree/Tree.hpp"
using namespace std;

static void random_forests() {
    mt19937 rng(912048);
    for (int iteration = 0; iteration < 150; ++iteration) {
        int n = 1 + rng() % 60, root = rng() % n;
        Tree<> tree(n);
        vector<vector<int>> graph(n);
        for (int v = 1; v < n; ++v) if (rng() % 4) {
            int p = rng() % v;
            tree.add_edge(p, v, (int)(rng() % 21) - 10);
            graph[p].push_back(v); graph[v].push_back(p);
        }
        tree.build(root);
        vector<int> parent(n, -2);
        auto visit = [&](int start) {
            if (parent[start] != -2) return;
            vector<int> queue{start}; parent[start] = -1;
            for (size_t i = 0; i < queue.size(); ++i) {
                int v = queue[i];
                for (int to : graph[v]) if (parent[to] == -2) {
                    parent[to] = v; queue.push_back(to);
                }
            }
        };
        visit(root);
        for (int v = 0; v < n; ++v) visit(v);
        auto in_subtree = [&](int v, int ancestor) {
            while (v != -1 && v != ancestor) v = parent[v];
            return v == ancestor;
        };
        auto path = [&](int start, int target) {
            vector<int> previous(n, -2), queue{start}; previous[start] = -1;
            for (size_t i = 0; i < queue.size(); ++i)
                for (int to : graph[queue[i]]) if (previous[to] == -2) {
                    previous[to] = queue[i]; queue.push_back(to);
                }
            vector<int> result;
            if (previous[target] != -2)
                for (int v = target; v != -1; v = previous[v]) result.push_back(v);
            return result;
        };
        vector<long long> expected(n);
        for (auto& value : expected) value = (int)(rng() % 101) - 50;
        auto values = tree.make_add_sum(expected);
        const auto zero = tree.make_add_sum();
        assert(zero.sum_all() == 0 && values.size() == n);
        for (int step = 0; step < 160; ++step) {
            int u = rng() % n, v = rng() % n, operation = rng() % 7;
            long long delta = (int)(rng() % 101) - 50;
            if (operation == 0) {
                values.add_subtree(u, delta);
                for (int x = 0; x < n; ++x) if (in_subtree(x, u)) expected[x] += delta;
            } else if (operation == 1) {
                auto vertices = path(u, v);
                assert(values.add_path(u, v, delta) == !vertices.empty());
                for (int x : vertices) expected[x] += delta;
            } else if (operation == 2) {
                values.set(u, delta); expected[u] = delta;
            } else if (operation == 3) {
                values.add_vertex(u, delta); expected[u] += delta;
            } else if (operation == 4) {
                auto vertices = path(u, v);
                auto actual = values.sum_path(u, v);
                if (vertices.empty()) assert(!actual);
                else {
                    long long sum = 0;
                    for (int x : vertices) sum += expected[x];
                    assert(actual && *actual == sum);
                }
            } else if (operation == 5) {
                long long sum = 0;
                for (int x = 0; x < n; ++x) if (in_subtree(x, u)) sum += expected[x];
                assert(values.sum_subtree(u) == sum);
            } else assert(values.get(u) == expected[u]);
            assert(values.sum_all() == accumulate(expected.begin(), expected.end(), 0LL));
        }
        // A later reroot of the source must not change existing value objects.
        tree.build((root + 1) % n);
        for (int v = 0; v < n; ++v) {
            long long sum = 0;
            for (int x = 0; x < n; ++x) if (in_subtree(x, v)) sum += expected[x];
            assert(values.sum_subtree(v) == sum && values.get(v) == expected[v]);
        }
        auto copy = values;
        copy.add_vertex(root, 17);
        assert(values.get(root) == expected[root] && copy.get(root) == expected[root] + 17);
    }
}

static void boundary_cases() {
    Tree<> empty; empty.build();
    const auto no_values = empty.make_add_sum();
    assert(no_values.size() == 0 && no_values.sum_all() == 0);
    auto independent = [] {
        Tree<> single(1); single.build();
        return single.make_add_sum(vector<long long>{-7});
    }();
    independent.add_subtree(0, 10);
    assert(independent.sum_path(0, 0) == optional<long long>(3));
    assert(independent.add_path(0, 0, -4) && independent.get(0) == -1);
    Tree<> pair(2); pair.add_edge(0, 1); pair.build();
    auto wide = pair.make_add_sum<__int128_t>();
    __int128_t big = (__int128_t)1 << 90;
    wide.add_subtree(0, big);
    assert(wide.sum_all() == big * 2 && wide.get(1) == big);

    const int n = 200000;
    Tree<> chain(n);
    for (int v = 1; v < n; ++v) chain.add_edge(v - 1, v);
    chain.build();
    auto values = chain.make_add_sum();
    values.add_subtree(n / 2, 3);
    assert(values.sum_subtree(0) == 3LL * (n - n / 2));
    assert(values.add_path(0, n - 1, 2));
    assert(values.sum_path(0, n - 1) == optional<long long>(2LL * n + 3LL * (n - n / 2)));
    assert(values.get(0) == 2 && values.get(n - 1) == 5);
}

int main() {
    random_forests(); boundary_cases();
    cout << "Hello World\n";
}
