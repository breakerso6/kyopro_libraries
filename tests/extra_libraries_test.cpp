#include <bits/stdc++.h>
using namespace std;

#include "libraries/data_structure/RollbackDSU.hpp"
#include "libraries/data_structure/WeightedDSU.hpp"
#include "libraries/data_structure/SparseTable.hpp"
#include "libraries/data_structure/DisjointSparseTable.hpp"
#include "libraries/data_structure/SWAG.hpp"
#include "libraries/data_structure/BinaryTrie.hpp"
#include "libraries/math/XorBasis.hpp"
#include "libraries/graph/LowLink.hpp"
#include "libraries/graph/EulerianTrail.hpp"
#include "libraries/tree/CentroidDecomposition.hpp"
#include "libraries/string/RollingHash.hpp"
#include "libraries/string/AhoCorasick.hpp"
#include "libraries/algorithm/Mo.hpp"
#include "libraries/data_structure/LiChaoTree.hpp"

struct Minimum { int operator()(int a, int b) const { return min(a, b); } };
struct Sum { long long operator()(long long a, long long b) const { return a + b; } };
struct Concat { string operator()(const string& a, const string& b) const { return a + b; } };

static void test_dsu() {
    RollbackDSU d(5);
    int s0 = d.snapshot();
    assert(d.merge(0, 1)); assert(d.merge(1, 2)); assert(!d.merge(0, 2));
    int s1 = d.snapshot();
    assert(d.size(0) == 3 && d.components == 3);
    assert(d.merge(3, 4)); d.rollback(s1);
    assert(!d.same(3, 4) && d.same(0, 2));
    d.rollback(s0); assert(d.components == 5 && !d.undo());

    WeightedDSU<long long> w(5);
    assert(w.merge(0, 1, 3)); assert(w.merge(1, 2, -5));
    assert(w.diff(0, 2) == -2 && w.diff(2, 0) == 2);
    assert(w.merge(0, 2, -2)); assert(!w.merge(0, 2, 7));
    assert(w.merge(3, 2, 10)); assert(w.diff(3, 1) == 15);
}

static void test_range_structures() {
    mt19937 rng(123456);
    vector<int> a(100);
    for (int& x : a) x = (int)rng() % 1000;
    SparseTable<int, Minimum> st(a);
    DisjointSparseTable<long long, Sum> dst(vector<long long>(a.begin(), a.end()));
    for (int l = 0; l < 100; ++l) for (int r = l + 1; r <= 100; ++r) {
        assert(st.prod(l, r) == *min_element(a.begin() + l, a.begin() + r));
        assert(dst.prod(l, r) == accumulate(a.begin() + l, a.begin() + r, 0LL));
    }
    SWAG<string, Concat> swag(""), empty("");
    deque<string> q;
    for (int t = 0; t < 500; ++t) {
        if (q.empty() || rng() % 3) { string x(1, 'a' + rng() % 26); q.push_back(x); swag.push(x); }
        else { q.pop_front(); swag.pop(); }
        string expected; for (auto& x : q) expected += x;
        assert(swag.prod() == expected && swag.size() == (int)q.size());
    }
    assert(empty.prod().empty());
}

static void test_bit_structures() {
    mt19937 rng(42);
    BinaryTrie<unsigned, 10> trie;
    multiset<unsigned> values;
    for (int t = 0; t < 2000; ++t) {
        unsigned x = rng() % 1024;
        if (values.empty() || rng() % 2) { trie.insert(x); values.insert(x); }
        else {
            bool expected = values.find(x) != values.end();
            assert(trie.erase(x) == expected);
            if (expected) values.erase(values.find(x));
        }
        assert(trie.size() == (int)values.size());
        if (!values.empty()) {
            unsigned mn = UINT_MAX, mx = 0;
            for (unsigned y : values) mn = min(mn, x ^ y), mx = max(mx, x ^ y);
            assert(trie.min_xor(x) == mn && trie.max_xor(x) == mx);
        }
    }
    XorBasis<unsigned, 8> basis;
    vector<unsigned> inserted{3, 5, 12, 9};
    for (auto x : inserted) basis.insert(x);
    set<unsigned> possible{0};
    for (auto x : inserted) { auto copy = possible; for (auto y : copy) possible.insert(x ^ y); }
    for (unsigned x = 0; x < 256; ++x) assert(basis.contains(x) == possible.count(x));
    assert(basis.max_xor() == *possible.rbegin());
}

static void check_trail(const vector<pair<int, int>>& edges, const EulerianTrail::Result& result, bool directed) {
    assert(result.vertices.size() == edges.size() + 1 && result.edge_ids.size() == edges.size());
    vector<int> seen(edges.size());
    for (int i = 0; i < (int)edges.size(); ++i) {
        int id = result.edge_ids[i]; assert(0 <= id && id < (int)edges.size() && !seen[id]); seen[id] = 1;
        auto [u, v] = edges[id];
        assert(directed ? (result.vertices[i] == u && result.vertices[i + 1] == v)
                        : ((result.vertices[i] == u && result.vertices[i + 1] == v) ||
                           (result.vertices[i] == v && result.vertices[i + 1] == u)));
    }
}

static void test_graphs() {
    LowLink low(7);
    low.add_edge(0, 1); low.add_edge(1, 2); low.add_edge(2, 0);
    low.add_edge(1, 3); low.add_edge(3, 4); low.add_edge(3, 4);
    low.add_edge(5, 6);
    low.build();
    sort(low.articulation.begin(), low.articulation.end());
    assert((low.articulation == vector<int>{1, 3}));
    for (auto& e : low.bridges) if (e.first > e.second) swap(e.first, e.second);
    sort(low.bridges.begin(), low.bridges.end());
    assert((low.bridges == vector<pair<int, int>>{{1, 3}, {5, 6}}));

    vector<pair<int, int>> ue{{0, 1}, {1, 2}, {2, 0}, {0, 3}};
    auto ut = EulerianTrail::undirected(4, ue); assert(ut); check_trail(ue, *ut, false);
    vector<pair<int, int>> de{{0, 1}, {1, 2}, {2, 0}, {0, 2}};
    auto dt = EulerianTrail::directed(3, de); assert(dt); check_trail(de, *dt, true);
    assert(!EulerianTrail::undirected(4, {{0, 1}, {0, 2}, {0, 3}}));
    assert(EulerianTrail::directed(3, {})->vertices.size() == 1);

    vector<vector<int>> tree(9);
    for (int v = 1; v < 9; ++v) { int p = (v - 1) / 2; tree[p].push_back(v); tree[v].push_back(p); }
    CentroidDecomposition cd(tree);
    assert(count(cd.parent.begin(), cd.parent.end(), -1) == 1);
    for (int v = 0; v < 9; ++v) if (cd.parent[v] != -1) assert(cd.depth[cd.parent[v]] + 1 == cd.depth[v]);
}

static void test_strings() {
    string s = "abracadabra";
    RollingHash h(s), h2("cadabra");
    assert(h.hash(4, 11) == h2.hash(0, 7));
    assert(RollingHash::concat(h.hash(0, 4), h.hash(4, 11), 7) == h.hash(0, 11));
    for (int l1 = 0; l1 <= (int)s.size(); ++l1)
        for (int l2 = 0; l2 <= (int)s.size(); ++l2)
            for (int len = 0; l1 + len <= (int)s.size() && l2 + len <= (int)s.size(); ++len)
                assert((h.hash(l1, l1 + len) == h.hash(l2, l2 + len)) ==
                       (s.substr(l1, len) == s.substr(l2, len)));

    AhoCorasick<> ac;
    vector<string> patterns{"he", "she", "hers", "his", "he"};
    for (auto& p : patterns) ac.add(p);
    ac.build();
    string text = "ahishershe";
    auto got = ac.count_matches(text);
    for (int i = 0; i < (int)patterns.size(); ++i) {
        int expected = 0;
        for (int p = 0; p + (int)patterns[i].size() <= (int)text.size(); ++p)
            expected += text.compare(p, patterns[i].size(), patterns[i]) == 0;
        assert(got[i] == expected);
    }
}

static void test_offline_and_lines() {
    mt19937 rng(9876); int n = 80;
    vector<int> a(n); for (int& x : a) x = rng() % 20;
    Mo mo(n); vector<pair<int, int>> queries;
    for (int i = 0; i < 300; ++i) { int l = rng() % n, r = rng() % n; if (l > r) swap(l, r); ++r; queries.push_back({l, r}); mo.add_query(l, r); }
    vector<int> answer(queries.size()), freq(20); int distinct = 0;
    auto add = [&](int i) { distinct += freq[a[i]]++ == 0; };
    auto erase = [&](int i) { distinct -= --freq[a[i]] == 0; };
    mo.run(add, add, erase, erase, [&](int id) { answer[id] = distinct; });
    for (int i = 0; i < (int)queries.size(); ++i) {
        set<int> expected(a.begin() + queries[i].first, a.begin() + queries[i].second);
        assert(answer[i] == (int)expected.size());
    }

    const long long INF = (1LL << 60);
    LiChaoTree<long long> lichao(-50, 51, INF);
    vector<pair<long long, long long>> lines;
    for (int i = 0; i < 100; ++i) {
        long long slope = (int)(rng() % 41) - 20, intercept = (int)(rng() % 201) - 100;
        lines.push_back({slope, intercept}); lichao.add_line(slope, intercept);
        for (long long x = -50; x <= 50; ++x) {
            long long expected = INF;
            for (auto [line_a, line_b] : lines) expected = min(expected, line_a * x + line_b);
            assert(lichao.min(x) == expected);
        }
    }
}

int main() {
    test_dsu(); test_range_structures(); test_bit_structures(); test_graphs();
    test_strings(); test_offline_and_lines();
    cout << "extra library tests passed\n";
}
