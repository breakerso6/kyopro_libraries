#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;

#include "libraries/algorithm/SMAWK.hpp"
#include "libraries/data_structure/CartesianTree.hpp"
#include "libraries/data_structure/ImplicitTreapBeats.hpp"
#include "libraries/data_structure/ImplicitTreap.hpp"
#include "libraries/data_structure/OfflineDynamicConnectivity.hpp"
#include "libraries/data_structure/SegmentTreeBeats.hpp"
#include "libraries/data_structure/SlopeTrick.hpp"
#include "libraries/data_structure/SplayTreeSequence.hpp"
#include "libraries/data_structure/WaveletMatrix.hpp"
#include "libraries/graph/BiconnectedComponents.hpp"
#include "libraries/math/Factorization.hpp"
#include "libraries/string/Manacher.hpp"
#include "libraries/string/PalindromicTree.hpp"
#include "libraries/tree/HLD.hpp"
#include "libraries/tree/LinkCutTree.hpp"
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

string op_string(string a, string b) { return a + b; }
string e_string() { return ""; }

static vector<int> bfs_path(int s, int t, const vector<set<int>>& graph) {
    vector<int> parent(graph.size(), -1);
    queue<int> que;
    parent[s] = s;
    que.push(s);
    while (!que.empty()) {
        int v = que.front();
        que.pop();
        if (v == t) break;
        for (int to : graph[v]) {
            if (parent[to] != -1) continue;
            parent[to] = v;
            que.push(to);
        }
    }
    if (parent[t] == -1) return {};
    vector<int> path;
    for (int v = t; v != s; v = parent[v]) path.push_back(v);
    path.push_back(s);
    reverse(path.begin(), path.end());
    return path;
}

struct TreapSum {
    long long sum;
    int size;
    bool operator==(const TreapSum& other) const {
        return sum == other.sum && size == other.size;
    }
};

TreapSum op_treap_sum(TreapSum a, TreapSum b) {
    return {a.sum + b.sum, a.size + b.size};
}

TreapSum e_treap_sum() {
    return {0, 0};
}

TreapSum mapping_treap_add(long long f, TreapSum x) {
    return {x.sum + f * x.size, x.size};
}

long long composition_treap_add(long long f, long long g) {
    return f + g;
}

long long id_treap_add() {
    return 0;
}

static void test_implicit_treap() {
    mt19937 rng(13579);
    vector<string> a;
    using StringTreap = ImplicitTreap<string, op_string, e_string>;
    StringTreap treap;
    vector<StringTreap::Handle> handles;
    for (int step = 0; step < 2000; ++step) {
        int type = rng() % 8;
        if (type == 0 || a.empty()) {
            int pos = rng() % (a.size() + 1);
            string value(1, char('a' + rng() % 26));
            a.insert(a.begin() + pos, value);
            handles.insert(handles.begin() + pos, treap.insert(pos, value));
        } else if (type == 1) {
            int pos = rng() % a.size();
            a.erase(a.begin() + pos);
            handles.erase(handles.begin() + pos);
            treap.erase(pos);
        } else if (type == 2) {
            int pos = rng() % a.size();
            string value(1, char('a' + rng() % 26));
            a[pos] = value;
            treap.set(pos, value);
        } else if (type == 3) {
            int l = rng() % (a.size() + 1), r = rng() % (a.size() + 1);
            if (l > r) swap(l, r);
            reverse(a.begin() + l, a.begin() + r);
            reverse(handles.begin() + l, handles.begin() + r);
            treap.reverse(l, r);
        } else if (type == 4) {
            int old_pos = rng() % a.size();
            string value = a[old_pos];
            auto handle = treap.extract(old_pos);
            assert(handle == handles[old_pos]);
            a.erase(a.begin() + old_pos);
            handles.erase(handles.begin() + old_pos);

            int new_pos = rng() % (a.size() + 1);
            if (rng() & 1) {
                value = string(1, char('a' + rng() % 26));
                treap.reinsert(new_pos, handle, value);
            } else {
                treap.reinsert(new_pos, handle);
            }
            a.insert(a.begin() + new_pos, value);
            handles.insert(handles.begin() + new_pos, handle);
        } else {
            int l = rng() % (a.size() + 1), r = rng() % (a.size() + 1);
            if (l > r) swap(l, r);
            string expected;
            for (int i = l; i < r; ++i) expected += a[i];
            assert(treap.prod(l, r) == expected);
        }
        assert(treap.size() == (int)a.size());
        for (int i = 0; i < (int)a.size(); ++i) {
            assert(treap.index_of(handles[i]) == i);
            assert(treap.handle_at(i) == handles[i]);
        }
        assert(treap.to_handles() == handles);
        assert(treap.to_vector() == a);
        for (int i = 0; i < (int)a.size(); ++i) assert(treap.get(i) == a[i]);
    }
    ImplicitTreap<string, op_string, e_string> built(vector<string>{"a", "b", "c", "d"});
    auto built_handles = built.to_handles();
    built.reverse(0, 4);
    assert(built.prod(0, 4) == "dcba");
    for (int i = 0; i < 4; ++i) assert(built.index_of(built_handles[i]) == 3 - i);

    vector<long long> b;
    using LazyTreap = ImplicitTreap<TreapSum, op_treap_sum, e_treap_sum, long long,
                                    mapping_treap_add, composition_treap_add, id_treap_add>;
    LazyTreap lazy_treap;
    vector<LazyTreap::Handle> lazy_handles;
    for (int step = 0; step < 3000; ++step) {
        int type = rng() % 9;
        if (type == 0 || b.empty()) {
            int pos = rng() % (b.size() + 1);
            long long value = (int)(rng() % 41) - 20;
            b.insert(b.begin() + pos, value);
            lazy_handles.insert(lazy_handles.begin() + pos, lazy_treap.insert(pos, {value, 1}));
        } else if (type == 1) {
            int pos = rng() % b.size();
            b.erase(b.begin() + pos);
            lazy_handles.erase(lazy_handles.begin() + pos);
            lazy_treap.erase(pos);
        } else if (type == 2) {
            int pos = rng() % b.size();
            long long value = (int)(rng() % 41) - 20;
            b[pos] = value;
            lazy_treap.set(pos, {value, 1});
        } else if (type == 3) {
            int l = rng() % (b.size() + 1), r = rng() % (b.size() + 1);
            if (l > r) swap(l, r);
            reverse(b.begin() + l, b.begin() + r);
            reverse(lazy_handles.begin() + l, lazy_handles.begin() + r);
            lazy_treap.reverse(l, r);
        } else if (type == 4) {
            int l = rng() % (b.size() + 1), r = rng() % (b.size() + 1);
            if (l > r) swap(l, r);
            long long add = (int)(rng() % 41) - 20;
            for (int i = l; i < r; ++i) b[i] += add;
            lazy_treap.apply(l, r, add);
        } else if (type == 5) {
            int old_pos = rng() % b.size();
            long long value = b[old_pos];
            auto handle = lazy_treap.extract(old_pos);
            assert(handle == lazy_handles[old_pos]);
            b.erase(b.begin() + old_pos);
            lazy_handles.erase(lazy_handles.begin() + old_pos);
            int new_pos = rng() % (b.size() + 1);
            lazy_treap.reinsert(new_pos, handle);
            b.insert(b.begin() + new_pos, value);
            lazy_handles.insert(lazy_handles.begin() + new_pos, handle);
        } else {
            int l = rng() % (b.size() + 1), r = rng() % (b.size() + 1);
            if (l > r) swap(l, r);
            long long expected = accumulate(b.begin() + l, b.begin() + r, 0LL);
            assert(lazy_treap.prod(l, r).sum == expected);
        }
        assert(lazy_treap.size() == (int)b.size());
        for (int i = 0; i < (int)b.size(); ++i) {
            assert(lazy_treap.index_of(lazy_handles[i]) == i);
            assert(lazy_treap.handle_at(i) == lazy_handles[i]);
        }
        assert(lazy_treap.to_handles() == lazy_handles);
        auto values = lazy_treap.to_vector();
        assert(values.size() == b.size());
        for (int i = 0; i < (int)b.size(); ++i) {
            assert(values[i] == (TreapSum{b[i], 1}));
            assert(lazy_treap.get(i) == (TreapSum{b[i], 1}));
        }
    }
}

static void test_implicit_treap_beats() {
    mt19937 rng(97531);
    ImplicitTreapBeats built(vector<long long>{5, 1, 8, 6});
    auto built_handles = built.to_handles();
    assert((built.to_vector() == vector<long long>{5, 1, 8, 6}));
    assert(built.range_sum(0, 4) == 20);
    assert(built.range_min(0, 4) == 1);
    assert(built.range_max(0, 4) == 8);
    built.reverse(0, 4);
    for (int i = 0; i < 4; ++i) assert(built.index_of(built_handles[i]) == 3 - i);
    built.reverse(0, 4);

    vector<long long> a;
    ImplicitTreapBeats treap;
    vector<ImplicitTreapBeats::Handle> handles;
    for (int step = 0; step < 5000; ++step) {
        int type = rng() % 12;
        if (type == 0 || a.empty()) {
            int pos = rng() % (a.size() + 1);
            long long value = (int)(rng() % 101) - 50;
            a.insert(a.begin() + pos, value);
            handles.insert(handles.begin() + pos, treap.insert(pos, value));
        } else if (type == 1) {
            int pos = rng() % a.size();
            a.erase(a.begin() + pos);
            handles.erase(handles.begin() + pos);
            treap.erase(pos);
        } else if (type == 2) {
            int pos = rng() % a.size();
            long long value = (int)(rng() % 101) - 50;
            a[pos] = value;
            treap.set(pos, value);
        } else {
            int l = rng() % (a.size() + 1), r = rng() % (a.size() + 1);
            if (l > r) swap(l, r);
            if (type == 3) {
                reverse(a.begin() + l, a.begin() + r);
                reverse(handles.begin() + l, handles.begin() + r);
                treap.reverse(l, r);
            } else if (type == 4) {
                long long x = (int)(rng() % 101) - 50;
                for (int i = l; i < r; ++i) a[i] = min(a[i], x);
                treap.range_chmin(l, r, x);
            } else if (type == 5) {
                long long x = (int)(rng() % 101) - 50;
                for (int i = l; i < r; ++i) a[i] = max(a[i], x);
                treap.range_chmax(l, r, x);
            } else if (type == 6) {
                long long x = (int)(rng() % 41) - 20;
                for (int i = l; i < r; ++i) a[i] += x;
                treap.range_add(l, r, x);
            } else if (type == 7) {
                int old_pos = rng() % a.size();
                long long value = a[old_pos];
                auto handle = treap.extract(old_pos);
                assert(handle == handles[old_pos]);
                a.erase(a.begin() + old_pos);
                handles.erase(handles.begin() + old_pos);
                int new_pos = rng() % (a.size() + 1);
                if (rng() & 1) {
                    value = (int)(rng() % 101) - 50;
                    treap.reinsert(new_pos, handle, value);
                } else {
                    treap.reinsert(new_pos, handle);
                }
                a.insert(a.begin() + new_pos, value);
                handles.insert(handles.begin() + new_pos, handle);
            } else {
                long long expected_sum = accumulate(a.begin() + l, a.begin() + r, 0LL);
                assert(treap.range_sum(l, r) == expected_sum);
                if (l < r) {
                    assert(treap.range_min(l, r) == *min_element(a.begin() + l, a.begin() + r));
                    assert(treap.range_max(l, r) == *max_element(a.begin() + l, a.begin() + r));
                }
            }
        }
        assert(treap.size() == (int)a.size());
        for (int i = 0; i < (int)a.size(); ++i) {
            assert(treap.index_of(handles[i]) == i);
            assert(treap.handle_at(i) == handles[i]);
        }
        assert(treap.to_handles() == handles);
        assert(treap.to_vector() == a);
        for (int i = 0; i < (int)a.size(); ++i) assert(treap.get(i) == a[i]);
        if (!a.empty()) {
            assert(treap.range_sum(0, a.size()) == accumulate(a.begin(), a.end(), 0LL));
            assert(treap.range_min(0, a.size()) == *min_element(a.begin(), a.end()));
            assert(treap.range_max(0, a.size()) == *max_element(a.begin(), a.end()));
        }
    }
}

static void test_link_cut_tree() {
    {
        const int n = 8;
        LinkCutTree<string, op_string, e_string> lct(vector<string>{"0", "1", "2", "3", "4", "5", "6", "7"});
        vector<set<int>> graph(n);
        auto add_edge = [&](int u, int v) {
            assert(lct.link(u, v));
            graph[u].insert(v);
            graph[v].insert(u);
        };
        add_edge(0, 1);
        add_edge(1, 2);
        add_edge(1, 3);
        add_edge(3, 4);
        add_edge(3, 5);
        add_edge(0, 6);
        add_edge(6, 7);
        for (int root = 0; root < n; ++root) {
            lct.evert(root);
            vector<int> parent(n, -1), depth(n, 0);
            queue<int> que;
            parent[root] = root;
            que.push(root);
            while (!que.empty()) {
                int v = que.front();
                que.pop();
                for (int to : graph[v]) {
                    if (parent[to] != -1) continue;
                    parent[to] = v;
                    depth[to] = depth[v] + 1;
                    que.push(to);
                }
            }
            auto naive_lca = [&](int u, int v) {
                while (depth[u] > depth[v]) u = parent[u];
                while (depth[v] > depth[u]) v = parent[v];
                while (u != v) {
                    u = parent[u];
                    v = parent[v];
                }
                return u;
            };
            for (int u = 0; u < n; ++u) for (int v = 0; v < n; ++v) {
                assert(lct.lca(u, v) == naive_lca(u, v));
            }
        }
    }

    mt19937 rng(86420);
    const int n = 45;
    vector<string> value(n);
    for (int i = 0; i < n; ++i) value[i] = string(1, char('a' + i % 26));
    LinkCutTree<string, op_string, e_string> lct(value);
    vector<set<int>> graph(n);

    for (int step = 0; step < 4000; ++step) {
        int type = rng() % 8;
        if (type == 0) {
            int u = rng() % n, v = rng() % n;
            if (u == v) continue;
            bool expected = bfs_path(u, v, graph).empty();
            bool actual = lct.link(u, v);
            assert(actual == expected);
            if (actual) {
                graph[u].insert(v);
                graph[v].insert(u);
            }
        } else if (type == 1) {
            int u = rng() % n, v = rng() % n;
            bool expected = graph[u].count(v);
            bool actual = lct.cut(u, v);
            assert(actual == expected);
            if (actual) {
                graph[u].erase(v);
                graph[v].erase(u);
            }
        } else if (type == 2) {
            int v = rng() % n;
            value[v] = string(1, char('A' + rng() % 26));
            lct.set(v, value[v]);
        } else {
            int u = rng() % n, v = rng() % n;
            auto path = bfs_path(u, v, graph);
            bool expected_connected = !path.empty();
            assert(lct.connected(u, v) == expected_connected);
            if (type == 3) {
                if (expected_connected) {
                    string expected;
                    for (int x : path) expected += value[x];
                    assert(lct.prod(u, v) == expected);
                }
            } else if (type == 4) {
                if (expected_connected) lct.evert(u);
            } else if (type == 5) {
                assert(lct.get(u) == value[u]);
            } else if (type == 6) {
                int actual = lct.root(u);
                assert(!bfs_path(u, actual, graph).empty());
                for (int x = 0; x < n; ++x) {
                    assert((!bfs_path(u, x, graph).empty()) == (!bfs_path(actual, x, graph).empty()));
                }
            } else {
                int w = lct.lca(u, v);
                assert((w != -1) == expected_connected);
                if (w != -1) assert(!bfs_path(u, w, graph).empty() && !bfs_path(v, w, graph).empty());
            }
        }
    }

    vector<long long> number(n);
    vector<TreapSum> initial(n);
    for (int i = 0; i < n; ++i) {
        number[i] = (int)(rng() % 101) - 50;
        initial[i] = {number[i], 1};
    }
    LinkCutTree<TreapSum, op_treap_sum, e_treap_sum, long long,
                mapping_treap_add, composition_treap_add, id_treap_add> lazy_lct(initial);
    vector<set<int>> graph2(n);
    for (int step = 0; step < 4000; ++step) {
        int type = rng() % 7;
        if (type == 0) {
            int u = rng() % n, v = rng() % n;
            if (u == v) continue;
            bool expected = bfs_path(u, v, graph2).empty();
            bool actual = lazy_lct.link(u, v);
            assert(actual == expected);
            if (actual) {
                graph2[u].insert(v);
                graph2[v].insert(u);
            }
        } else if (type == 1) {
            int u = rng() % n, v = rng() % n;
            bool expected = graph2[u].count(v);
            bool actual = lazy_lct.cut(u, v);
            assert(actual == expected);
            if (actual) {
                graph2[u].erase(v);
                graph2[v].erase(u);
            }
        } else if (type == 2) {
            int v = rng() % n;
            number[v] = (int)(rng() % 101) - 50;
            lazy_lct.set(v, {number[v], 1});
        } else {
            int u = rng() % n, v = rng() % n;
            auto path = bfs_path(u, v, graph2);
            bool expected_connected = !path.empty();
            assert(lazy_lct.connected(u, v) == expected_connected);
            if (!expected_connected) continue;
            if (type == 3) {
                long long add = (int)(rng() % 41) - 20;
                for (int x : path) number[x] += add;
                lazy_lct.apply(u, v, add);
            } else if (type == 4) {
                long long expected = 0;
                for (int x : path) expected += number[x];
                assert(lazy_lct.prod(u, v).sum == expected);
            } else if (type == 5) {
                lazy_lct.evert(u);
            } else {
                int x = rng() % n;
                assert(lazy_lct.get(x) == (TreapSum{number[x], 1}));
            }
        }
    }
}

template<class Sequence>
static void random_sequence_test() {
    mt19937 rng(24680);
    vector<string> a;
    Sequence sequence;
    for (int step = 0; step < 2000; ++step) {
        int type = rng() % 7;
        if (type == 0 || a.empty()) {
            int pos = rng() % (a.size() + 1);
            string value(1, char('a' + rng() % 26));
            a.insert(a.begin() + pos, value);
            sequence.insert(pos, value);
        } else if (type == 1) {
            int pos = rng() % a.size();
            a.erase(a.begin() + pos);
            sequence.erase(pos);
        } else if (type == 2) {
            int pos = rng() % a.size();
            string value(1, char('a' + rng() % 26));
            a[pos] = value;
            sequence.set(pos, value);
        } else {
            int l = rng() % (a.size() + 1), r = rng() % (a.size() + 1);
            if (l > r) swap(l, r);
            if (type == 3) {
                reverse(a.begin() + l, a.begin() + r);
                sequence.reverse(l, r);
            } else {
                string expected;
                for (int i = l; i < r; ++i) expected += a[i];
                assert(sequence.prod(l, r) == expected);
            }
        }
        assert(sequence.size() == (int)a.size());
        assert(sequence.to_vector() == a);
        for (int i = 0; i < (int)a.size(); ++i) assert(sequence.get(i) == a[i]);
    }
}

static void test_splay_tree_sequence() {
    random_sequence_test<SplayTreeSequence<string, op_string, e_string>>();
    SplayTreeSequence<string, op_string, e_string> built(vector<string>{"a", "b", "c", "d"});
    built.reverse(0, 4);
    assert(built.prod(0, 4) == "dcba");
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
    test_implicit_treap(); test_implicit_treap_beats(); test_splay_tree_sequence(); test_link_cut_tree(); test_number_theory(); test_palindromes(); test_graph_components(); test_tree_and_optimization();
    cout << "Hello World\n";
}
