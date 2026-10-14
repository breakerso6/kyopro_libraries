#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include <bits/stdc++.h>
#include "libraries/algorithm/SMAWK.hpp"
#include "libraries/data_structure/LiChaoTree.hpp"
#include "libraries/data_structure/PersistentSegmentTree.hpp"
#include "libraries/data_structure/DynamicSegmentTree.hpp"
#include "libraries/graph/HopcroftKarp.hpp"

using namespace std;
string concat(string a, string b) { return a + b; }
string empty_string() { return ""; }

static void test_smawk() {
    mt19937 rng(1709);
    for (int rows : {0, 1, 2, 3, 4, 17, 64}) for (int columns : {0, 1, 2, 5, 8, 51}) {
        vector<int> centers(rows);
        for (int& center : centers) center = (int)(rng() % 101) - 20;
        sort(centers.begin(), centers.end());
        vector<int> bias(columns);
        for (int& b : bias) b = (int)(rng() % 101) - 50;
        // Increasing centers plus arbitrary column biases remains Monge.
        auto cost = [&](int row, int column) {
            long long delta = centers[row] - column;
            return delta * delta + bias[column];
        };
        auto result = smawk(rows, columns, cost);
        for (int row = 0; row < rows; ++row) {
            int expected = columns ? 0 : -1;
            for (int column = 1; column < columns; ++column)
                if (cost(row, column) < cost(row, expected)) expected = column;
            assert(result[row] == expected);
        }
        auto ties = smawk(rows, columns, [](int, int) { return 0; });
        for (int column : ties) assert(column == (columns ? 0 : -1));
    }
    int calls = 0, n = 100000;
    auto result = smawk(n, n, [&](int row, int column) {
        ++calls;
        long long delta = row - column;
        return delta * delta;
    });
    for (int row = 0; row < n; ++row) assert(result[row] == row);
    assert(calls < 30 * n);
}

static void test_li_chao() {
    mt19937 rng(77);
    for (int width : {1, 2, 3, 7, 31, 64}) {
        const long long inf = 1000000000000LL;
        LiChaoTree<long long> tree(-17, -17 + width, inf);
        vector<long long> expected(width, inf);
        for (int trial = 0; trial < 300; ++trial) {
            long long a = (int)(rng() % 101) - 50, b = (int)(rng() % 101) - 50;
            tree.add_line(a, b);
            for (int i = 0; i < width; ++i) {
                expected[i] = min(expected[i], a * (-17 + i) + b);
                assert(tree.min(-17 + i) == expected[i]);
            }
        }
    }
    LiChaoTree<long long> same(-1000000000, 1000000000, (long long)4e18);
    for (int i = 0; i < 10000; ++i) same.add_line(7, 10000 - i);
    assert(same.min(-999999999) == 7 * -999999999LL + 1);
    // Same slopes replace the line without expanding the domain's tree.
    assert(same.nodes.size() <= 2);
    LiChaoTree<long long> ties(0, 3, 1000000);
    ties.add_line(1, 0); ties.add_line(-1, 2); ties.add_line(0, 1);
    for (long long x = 0; x < 3; ++x) assert(ties.min(x) == min(x, 2 - x));
    const int n = 1024;
    LiChaoTree<long long> active(0, n, 1000000000000LL);
    vector<int> order(n); iota(order.begin(), order.end(), 0); shuffle(order.begin(), order.end(), rng);
    vector<long long> expected(n, 1000000000000LL);
    for (long long i : order) {
        active.add_line(-2 * i, i * i);
        for (int x = 0; x < n; ++x) expected[x] = min(expected[x], -2 * i * x + i * i);
        int x = rng() % n;
        assert(active.min(x) == expected[x]);
    }
    for (long long x = 0; x < n; ++x) assert(active.min(x) == -x * x);
}

static void test_segment_trees() {
    mt19937 rng(112233);
    for (int n : {1, 2, 3, 7, 16, 31}) {
        vector<string> a(n);
        for (auto& s : a) s = string(1, 'a' + rng() % 26);
        PersistentSegmentTree<string, concat, empty_string> persistent(n);
        vector<int> roots{persistent.build(a)};
        vector<vector<string>> versions{a};
        DynamicSegmentTree<string, concat, empty_string> dynamic(-5, n - 5);
        for (int i = 0; i < n; ++i) dynamic.set(i - 5, a[i]);
        for (int trial = 0; trial < 150; ++trial) {
            int old = rng() % roots.size(), index = rng() % n;
            string value = rng() % 3 ? string(1, 'a' + rng() % 26) : "";
            roots.push_back(persistent.set(roots[old], index, value));
            auto next = versions[old]; next[index] = value; versions.push_back(next);
            a[index] = value; dynamic.set(index - 5, value);
            int version = rng() % roots.size();
            for (int i = 0; i < n; ++i) {
                assert(persistent.get(roots[version], i) == versions[version][i]);
                assert(dynamic.get(i - 5) == a[i]);
            }
            for (int l = 0; l <= n; ++l) for (int r = l; r <= n; ++r) {
                string historical, current;
                for (int i = l; i < r; ++i) historical += versions[version][i], current += a[i];
                assert(persistent.prod(roots[version], l, r) == historical);
                assert(dynamic.prod(l - 5, r - 5) == current);
            }
        }
        // Repeated builds must retain old versions across vector reallocations.
        for (int i = 0; i < 10; ++i) assert(persistent.prod(persistent.build(), 0, n).empty());
        assert(persistent.prod(roots[0], 0, n) == accumulate(versions[0].begin(), versions[0].end(), string()));
    }
    DynamicSegmentTree<string, concat, empty_string> sparse(-1000000000000LL, 1000000000000LL);
    assert(sparse.get(123456789) == "");
    sparse.set(999999999999LL, "z");
    assert(sparse.get(999999999999LL) == "z");
    assert(sparse.prod(-1000000000000LL, 1000000000000LL) == "z");
}

static int brute_matching(const vector<vector<int>>& graph, int v, int used) {
    if (v == (int)graph.size()) return 0;
    int answer = brute_matching(graph, v + 1, used);
    for (int to : graph[v]) if (!(used & (1 << to)))
        answer = max(answer, 1 + brute_matching(graph, v + 1, used | (1 << to)));
    return answer;
}

static void check_matching(HopcroftKarp& graph) {
    int result = graph.max_matching();
    if (graph.left_size <= 7 && graph.right_size <= 7)
        assert(result == brute_matching(graph.graph, 0, 0));
    assert(graph.max_matching() == result);
    int matched = 0;
    for (int v = 0; v < graph.left_size; ++v) if (graph.match_left[v] != -1) {
        int to = graph.match_left[v]; ++matched;
        assert(graph.match_right[to] == v);
        assert(find(graph.graph[v].begin(), graph.graph[v].end(), to) != graph.graph[v].end());
    }
    assert(matched == result);
    auto [left, right] = graph.minimum_vertex_cover();
    assert((int)(left.size() + right.size()) == result);
    for (int v = 0; v < graph.left_size; ++v) for (int to : graph.graph[v])
        assert(find(left.begin(), left.end(), v) != left.end() || find(right.begin(), right.end(), to) != right.end());
}

static void test_matching() {
    mt19937 rng(4242);
    for (int trial = 0; trial < 200; ++trial) {
        int n = rng() % 7, m = rng() % 7;
        HopcroftKarp graph(n, m);
        check_matching(graph);
        for (int step = 0; step < 20 && n && m; ++step) {
            graph.add_edge(rng() % n, rng() % m);
            check_matching(graph);
        }
    }
    // A valid matching and a vertex cover of the same size certify optimality
    // independently of the augmentation implementation, also on larger graphs.
    for (int trial = 0; trial < 50; ++trial) {
        int n = 20 + rng() % 80, m = 10 + rng() % 100;
        HopcroftKarp graph(n, m);
        for (int phase = 0; phase < 4; ++phase) {
            for (int edge = 0; edge < 200; ++edge) graph.add_edge(rng() % n, rng() % m);
            check_matching(graph);
        }
    }
    HopcroftKarp chain(2000, 2000);
    for (int i = 0; i < 1999; ++i) {
        chain.add_edge(i, i); chain.add_edge(i, i + 1);
        chain.match_left[i] = i; chain.match_right[i] = i;
    }
    chain.add_edge(1999, 0);
    assert(chain.max_matching() == 2000);
    assert(chain.minimum_vertex_cover().first.size() == 2000);
}

int main() {
    test_smawk(); test_li_chao(); test_segment_trees(); test_matching();
    cout << "Hello World\n";
}
