#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;

#include "libraries/algorithm/SMAWK.hpp"
#include "libraries/data_structure/CartesianTree.hpp"
#include "libraries/data_structure/OfflineDynamicConnectivity.hpp"
#include "libraries/data_structure/SegmentTreeBeats.hpp"
#include "libraries/data_structure/SlopeTrick.hpp"
#include "libraries/data_structure/WaveletMatrix.hpp"
#include "libraries/graph/BiconnectedComponents.hpp"
#include "libraries/math/Factorization.hpp"
#include "libraries/string/Manacher.hpp"
#include "libraries/string/PalindromicTree.hpp"
#include "libraries/tree/HLD.hpp"
#include "libraries/tree/VirtualTree.hpp"

static void test_wavelet_matrix() {
    WaveletMatrix<int> power_of_two_sigma(vector<int>{0, 1, 2, 3});
    assert(power_of_two_sigma.count_less(0, 4, 100) == 4);
    mt19937 rng(1234);
    vector<long long> a(80); for (auto& x : a) x = (int)(rng() % 31) - 15;
    WaveletMatrix<long long> wm(a);
    for (int l = 0; l < 80; ++l) for (int r = l + 1; r <= 80; ++r) {
        vector<long long> sorted(a.begin() + l, a.begin() + r); sort(sorted.begin(), sorted.end());
        for (int k = 0; k < r - l; ++k) assert(wm.kth_smallest(l, r, k) == sorted[k]);
        for (int x = -18; x <= 18; ++x) {
            assert(wm.count_less(l, r, x) == lower_bound(sorted.begin(), sorted.end(), x) - sorted.begin());
            assert(wm.count(l, r, x) == count(sorted.begin(), sorted.end(), x));
            auto prev = wm.prev_value(l, r, x), next = wm.next_value(l, r, x);
            auto it = lower_bound(sorted.begin(), sorted.end(), x);
            assert(bool(prev) == (it != sorted.begin())); if (prev) assert(*prev == *std::prev(it));
            assert(bool(next) == (it != sorted.end())); if (next) assert(*next == *it);
        }
    }
}

static void test_segment_tree_beats() {
    mt19937 rng(5678); int n = 73;
    vector<long long> a(n); for (auto& x : a) x = (int)(rng() % 101) - 50;
    SegmentTreeBeats seg(a);
    for (int step = 0; step < 4000; ++step) {
        int l = rng() % n, r = rng() % n; if (l > r) swap(l, r); ++r;
        long long x = (int)(rng() % 101) - 50; int type = rng() % 6;
        if (type == 0) { seg.range_chmin(l, r, x); for (int i = l; i < r; ++i) a[i] = min(a[i], x); }
        else if (type == 1) { seg.range_chmax(l, r, x); for (int i = l; i < r; ++i) a[i] = max(a[i], x); }
        else if (type == 2) { seg.range_add(l, r, x); for (int i = l; i < r; ++i) a[i] += x; }
        else if (type == 3) assert(seg.range_sum(l, r) == accumulate(a.begin() + l, a.begin() + r, 0LL));
        else if (type == 4) assert(seg.range_min(l, r) == *min_element(a.begin() + l, a.begin() + r));
        else assert(seg.range_max(l, r) == *max_element(a.begin() + l, a.begin() + r));
    }
}

static void test_offline_connectivity() {
    OfflineDynamicConnectivity dc(5);
    dc.add_edge(0, 1); int q0 = dc.same_query(0, 2);
    dc.add_edge(1, 2); int q1 = dc.same_query(0, 2); int q2 = dc.size_query(1);
    dc.add_edge(0, 1); dc.remove_edge(0, 1); int q3 = dc.same_query(0, 2);
    dc.remove_edge(0, 1); int q4 = dc.same_query(0, 2); int q5 = dc.components_query();
    auto answer = dc.run();
    assert(answer[q0] == 0 && answer[q1] == 1 && answer[q2] == 3);
    assert(answer[q3] == 1 && answer[q4] == 0 && answer[q5] == 4);
}

static void test_number_theory() {
    using namespace factorization;
    assert(is_prime(2) && is_prime(18446744073709551557ULL));
    assert(!is_prime(1) && !is_prime(3215031751ULL));
    u64 n = 1000000007ULL * 1000000009ULL;
    assert((prime_factors(n) == vector<u64>{1000000007ULL, 1000000009ULL}));
    assert((factorize(360) == vector<pair<u64, int>>{{2, 3}, {3, 2}, {5, 1}}));
    assert((divisors(12) == vector<u64>{1, 2, 3, 4, 6, 12}));
    assert(divisor_count(360) == 24);
    assert(divisor_sum(360) == 1170);
    assert(euler_phi(360) == 96);
    assert(mobius(1) == 1);
    assert(mobius(30) == -1);
    assert(mobius(12) == 0);
    assert(is_square_free(30));
    assert(!is_square_free(12));
    assert(radical(360) == 30);
    using mint = atcoder::modint998244353;
    assert(divisor_sum<mint>(360).val() == 1170);
}

static void test_palindromes() {
    mt19937 rng(9012);
    for (int n = 0; n <= 50; ++n) {
        string s(n, 'a'); for (char& c : s) c += rng() % 3;
        Manacher manacher(s); set<string> distinct;
        for (int l = 0; l <= n; ++l) for (int r = l; r <= n; ++r) {
            string t = s.substr(l, r - l), reversed = t; reverse(reversed.begin(), reversed.end());
            assert(manacher.is_palindrome(l, r) == (t == reversed));
            if (!t.empty() && t == reversed) distinct.insert(t);
        }
        PalindromicTree tree(s); assert(tree.distinct_count() == (int)distinct.size());
        tree.propagate_occurrences();
        for (int node = 2; node < (int)tree.nodes.size(); ++node) {
            string p = tree.palindrome(node); long long expected = 0;
            for (int i = 0; i + (int)p.size() <= n; ++i) expected += s.compare(i, p.size(), p) == 0;
            assert(tree.nodes[node].occurrences == expected);
        }
    }
}

static void test_graph_components() {
    vector<pair<int, int>> edges{{0,1},{1,2},{2,0},{1,3},{3,4},{3,4},{4,5}};
    TwoEdgeConnectedComponents two(7); VertexBiconnectedComponents vertex(7);
    for (auto [u,v] : edges) two.add_edge(u,v), vertex.add_edge(u,v);
    two.build(); vertex.build();
    assert(two.component[0] == two.component[2]); assert(two.component[3] == two.component[4]);
    assert(two.component[4] != two.component[5]); assert(two.component[6] != two.component[5]);
    assert(count(two.is_bridge.begin(), two.is_bridge.end(), true) == 2);
    assert((vertex.articulation == vector<int>{1,3,4}));
    assert(vertex.vertex_node[6] >= 0);
    int block_edges = 0; for (auto& adjacent : vertex.block_cut_tree) block_edges += adjacent.size();
    assert(block_edges / 2 == 6);
}

static void test_tree_and_optimization() {
    vector<vector<long long>> graph(9);
    for (int v = 1; v < 9; ++v) { int p = (v - 1) / 2; graph[p].push_back(v); graph[v].push_back(p); }
    HLD hld(graph); auto vt = VirtualTree::build(vector<int>{3,4,7,8}, hld);
    set<int> vertices(vt.vertices.begin(), vt.vertices.end());
    assert(vertices == set<int>({1,3,4,7,8}));
    assert(vt.parent[0] == -1);
    for (int i = 1; i < (int)vt.vertices.size(); ++i)
        assert(vt.distance_to_parent[i] == hld.depth[vt.vertices[i]] - hld.depth[vt.vertices[vt.parent[i]]]);

    vector<int> a{3,1,1,4,0,2}; CartesianTree<int> cartesian(a);
    assert(cartesian.root == 4);
    function<vector<int>(int)> inorder = [&](int v) { vector<int> result; if (v == -1) return result;
        auto l = inorder(cartesian.left[v]), r = inorder(cartesian.right[v]);
        result.insert(result.end(), l.begin(), l.end()); result.push_back(v); result.insert(result.end(), r.begin(), r.end()); return result; };
    assert((inorder(cartesian.root) == vector<int>{0,1,2,3,4,5}));
    for (int v = 0; v < (int)a.size(); ++v) if (cartesian.parent[v] != -1) assert(a[cartesian.parent[v]] <= a[v]);

    SlopeTrick<long long> slope; vector<long long> points{5,-2,8,5};
    for (long long x : points) slope.add_abs(x);
    assert(slope.min_f() == 10); auto interval = slope.argmin(); assert(interval.first == 5 && interval.second == 5);
    slope.shift(-2, 3); auto shifted = slope.argmin(); assert(shifted.first == 3 && shifted.second == 8);

    int rows = 40, columns = 55;
    auto value = [](int row, int column) { long long d = column - (row * 3 / 2); return d * d + row; };
    auto minima = smawk(rows, columns, value);
    for (int row = 0; row < rows; ++row) {
        int expected = 0;
        for (int column = 1; column < columns; ++column) if (value(row, column) < value(row, expected)) expected = column;
        assert(minima[row] == expected);
    }
}

int main() {
    test_wavelet_matrix(); test_segment_tree_beats(); test_offline_connectivity();
    test_number_theory(); test_palindromes(); test_graph_components(); test_tree_and_optimization();
    cout << "additional library tests passed\n";
}
