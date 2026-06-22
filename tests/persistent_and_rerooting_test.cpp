#include <bits/stdc++.h>
using namespace std;

#include "libraries/tree/RerootingDP.hpp"
#include "libraries/data_structure/PersistentSegmentTree.hpp"
#include "libraries/data_structure/PersistentLazySegmentTree.hpp"
#include "libraries/data_structure/PersistentDSU.hpp"
#include "libraries/data_structure/PersistentBinaryTrie.hpp"

struct SumOp { long long operator()(long long a, long long b) const { return a + b; } };

static void test_rerooting() {
    using DP = pair<long long, long long>; // {vertex count, sum of distances}
    auto merge = [](DP a, DP b) { return DP{a.first + b.first, a.second + b.second}; };
    auto add_vertex = [](DP value, int) { ++value.first; return value; };
    auto add_edge = [](DP value, long long weight, int, int) {
        value.second += value.first * weight; return value;
    };
    mt19937 rng(20260623);
    for (int n = 1; n <= 100; ++n) for (int trial = 0; trial < 10; ++trial) {
        auto rerooting = make_rerooting_dp<DP, long long>(n, DP{0, 0}, merge, add_vertex, add_edge);
        vector<vector<pair<int, int>>> graph(n);
        for (int v = 1; v < n; ++v) {
            int p = rng() % v, w = 1 + rng() % 20;
            rerooting.add_edge(p, v, w);
            graph[p].push_back({v, w}); graph[v].push_back({p, w});
        }
        auto answer = rerooting.build(rng() % n);
        for (int root = 0; root < n; ++root) {
            vector<long long> distance(n, -1); distance[root] = 0;
            vector<int> stack{root};
            for (int i = 0; i < n; ++i) {
                int v = stack[i];
                for (auto [to, w] : graph[v]) if (distance[to] == -1) {
                    distance[to] = distance[v] + w; stack.push_back(to);
                }
            }
            assert(answer[root] == DP(n, accumulate(distance.begin(), distance.end(), 0LL)));
        }
    }

    // Directional edge data: importing across 0->1 and 1->0 has different cost.
    auto directional = make_rerooting_dp<DP, long long>(2, DP{0, 0}, merge, add_vertex, add_edge);
    directional.add_edge(0, 1, 7LL, 11LL);
    auto answer = directional.build();
    assert(answer[0].second == 7 && answer[1].second == 11);

    auto concatenate = [](string a, const string& b) { return a + b; };
    auto wrap_vertex = [](string value, int v) { return "(" + to_string(v) + value + ")"; };
    auto label_edge = [](string value, char label, int, int) { return string(1, label) + value; };
    auto ordered = make_rerooting_dp<string, char>(5, "", concatenate, wrap_vertex, label_edge);
    ordered.add_edge(0, 1, 'a', 'A'); ordered.add_edge(0, 2, 'b', 'B');
    ordered.add_edge(1, 3, 'c', 'C'); ordered.add_edge(1, 4, 'd', 'D');
    auto ordered_answer = ordered.build(3);
    function<string(int, int)> brute = [&](int v, int parent) {
        string children;
        for (const auto& edge : ordered.graph[v]) if (edge.to != parent)
            children += label_edge(brute(edge.to, v), edge.data, edge.to, v);
        return wrap_vertex(children, v);
    };
    for (int root = 0; root < 5; ++root) assert(ordered_answer[root] == brute(root, -1));
}

static void test_persistent_segment_tree() {
    mt19937 rng(1234); const int n = 60;
    vector<long long> initial(n);
    for (auto& x : initial) x = (int)(rng() % 201) - 100;
    PersistentSegmentTree<long long, SumOp> tree(n, 0LL);
    vector<int> roots{tree.build(initial)};
    vector<vector<long long>> versions{initial};
    for (int trial = 0; trial < 3000; ++trial) {
        int base = rng() % roots.size(), position = rng() % n;
        long long value = (int)(rng() % 2001) - 1000;
        auto next = versions[base]; next[position] = value;
        roots.push_back(tree.set(roots[base], position, value)); versions.push_back(next);
        for (int check = 0; check < 5; ++check) {
            int version = rng() % roots.size(), l = rng() % (n + 1), r = rng() % (n + 1);
            if (l > r) swap(l, r);
            long long expected = accumulate(versions[version].begin() + l, versions[version].begin() + r, 0LL);
            assert(tree.prod(roots[version], l, r) == expected);
            int p = rng() % n; assert(tree.get(roots[version], p) == versions[version][p]);
        }
    }
}

static void test_persistent_lazy_segment_tree() {
    mt19937 rng(5678); const int n = 50;
    vector<long long> initial(n);
    for (auto& x : initial) x = (int)(rng() % 101) - 50;
    PersistentLazySegmentTree<long long> tree(n);
    vector<int> roots{tree.build(initial)};
    vector<vector<long long>> versions{initial};
    for (int trial = 0; trial < 2500; ++trial) {
        int base = rng() % roots.size(), l = rng() % (n + 1), r = rng() % (n + 1);
        if (l > r) swap(l, r);
        long long value = (int)(rng() % 101) - 50;
        auto next = versions[base]; for (int i = l; i < r; ++i) next[i] += value;
        roots.push_back(tree.add(roots[base], l, r, value)); versions.push_back(next);
        for (int check = 0; check < 5; ++check) {
            int version = rng() % roots.size(), ql = rng() % (n + 1), qr = rng() % (n + 1);
            if (ql > qr) swap(ql, qr);
            long long expected = accumulate(versions[version].begin() + ql, versions[version].begin() + qr, 0LL);
            assert(tree.prod(roots[version], ql, qr) == expected);
        }
    }
}

static void test_persistent_dsu() {
    mt19937 rng(9012); const int n = 30;
    PersistentDSU dsu(n);
    vector<int> roots{dsu.build()};
    vector<vector<int>> groups(1, vector<int>(n)); iota(groups[0].begin(), groups[0].end(), 0);
    for (int trial = 0; trial < 3000; ++trial) {
        int base = rng() % roots.size(), a = rng() % n, b = rng() % n;
        auto next = groups[base]; int old_group = next[b], new_group = next[a];
        for (int& group : next) if (group == old_group) group = new_group;
        roots.push_back(dsu.merge(roots[base], a, b)); groups.push_back(next);
        for (int check = 0; check < 10; ++check) {
            int version = rng() % roots.size(), x = rng() % n, y = rng() % n;
            assert(dsu.same(roots[version], x, y) == (groups[version][x] == groups[version][y]));
            int expected_size = count(groups[version].begin(), groups[version].end(), groups[version][x]);
            assert(dsu.size(roots[version], x) == expected_size);
        }
    }
}

static void test_persistent_binary_trie() {
    mt19937 rng(3456); const int bits = 10, mask = (1 << bits) - 1;
    PersistentBinaryTrie<unsigned, bits> trie;
    vector<int> prefix_roots{trie.empty_root()};
    vector<unsigned> values;
    for (int i = 0; i < 500; ++i) {
        values.push_back(rng() & mask);
        prefix_roots.push_back(trie.insert(prefix_roots.back(), values.back()));
    }
    for (int trial = 0; trial < 5000; ++trial) {
        int l = rng() % values.size(), r = l + 1 + rng() % (values.size() - l);
        unsigned x = rng() & mask;
        vector<unsigned> expected;
        for (int i = l; i < r; ++i) expected.push_back(values[i] ^ x);
        sort(expected.begin(), expected.end());
        int k = rng() % expected.size();
        assert(trie.kth_xor(prefix_roots[l], prefix_roots[r], x, k) == expected[k]);
        assert(trie.min_xor(prefix_roots[l], prefix_roots[r], x) == expected.front());
        assert(trie.max_xor(prefix_roots[l], prefix_roots[r], x) == expected.back());
    }
    int root = trie.insert(prefix_roots[100], values[0]);
    int erased = trie.erase(root, values[0]);
    assert(trie.count(erased, values[0]) == trie.count(prefix_roots[100], values[0]));
    assert(trie.size(root) == trie.size(prefix_roots[100]) + 1);
}

int main() {
    test_rerooting(); test_persistent_segment_tree(); test_persistent_lazy_segment_tree();
    test_persistent_dsu(); test_persistent_binary_trie();
    cout << "persistent and rerooting tests passed\n";
}
