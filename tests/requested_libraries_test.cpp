#include <bits/stdc++.h>
using namespace std;

#include "libraries/algorithm/Kitamasa.hpp"
#include "libraries/algorithm/ZAlgorithm.hpp"
#include "libraries/data_structure/ConvexHullTrick.hpp"
#include "libraries/data_structure/DynamicSegmentTree.hpp"
#include "libraries/data_structure/LazySegmentTree2D.hpp"
#include "libraries/data_structure/SegmentTree2D.hpp"
#include "libraries/graph/Dijkstra.hpp"
#include "libraries/graph/MST.hpp"
#include "libraries/math/MappingTwelvefold.hpp"
#include "libraries/math/Matrix.hpp"
#include "libraries/math/Mobius.hpp"

struct Sum {
    long long operator()(long long a, long long b) const { return a + b; }
};

struct Mint {
    static constexpr long long MOD = 998244353;
    long long v;
    Mint(long long x = 0) : v((x % MOD + MOD) % MOD) {}
    Mint operator+(Mint o) const { return v + o.v; }
    Mint operator-(Mint o) const { return v - o.v; }
    Mint operator-() const { return -v; }
    Mint operator*(Mint o) const { return v * o.v; }
    Mint pow(long long e) const { Mint a = *this, r = 1; while (e) { if (e & 1) r = r * a; a = a * a; e >>= 1; } return r; }
    Mint operator/(Mint o) const { return *this * o.pow(MOD - 2); }
    Mint& operator+=(Mint o) { return *this = *this + o; }
    Mint& operator-=(Mint o) { return *this = *this - o; }
    Mint& operator*=(Mint o) { return *this = *this * o; }
    bool operator==(Mint o) const { return v == o.v; }
    bool operator!=(Mint o) const { return v != o.v; }
};

static void test_dynamic_segment_tree() {
    DynamicSegmentTree<long long, Sum> seg(-1000000000000LL, 1000000000000LL, 0);
    map<long long, long long> mp;
    vector<long long> xs{-10, 0, 5, 999999999999LL, -999999999999LL};
    for (int i = 0; i < (int)xs.size(); ++i) seg.set(xs[i], i + 1), mp[xs[i]] = i + 1;
    for (long long l = -12; l <= 8; ++l) for (long long r = l; r <= 9; ++r) {
        long long expected = 0;
        for (auto [x, v] : mp) if (l <= x && x < r) expected += v;
        assert(seg.prod(l, r) == expected);
    }
}

static void test_2d_structures() {
    mt19937 rng(1);
    vector<vector<long long>> a(8, vector<long long>(7));
    for (auto& row : a) for (auto& x : row) x = (int)rng() % 20 - 10;
    SegmentTree2D<long long, Sum> seg(a, 0);
    for (int x1 = 0; x1 <= 8; ++x1) for (int x2 = x1; x2 <= 8; ++x2)
        for (int y1 = 0; y1 <= 7; ++y1) for (int y2 = y1; y2 <= 7; ++y2) {
            long long expected = 0;
            for (int i = x1; i < x2; ++i) for (int j = y1; j < y2; ++j) expected += a[i][j];
            assert(seg.prod(x1, x2, y1, y2) == expected);
        }
    RangeAddRangeSum2D<long long> lazy(a);
    for (int t = 0; t < 200; ++t) {
        int x1 = rng() % 9, x2 = rng() % 9, y1 = rng() % 8, y2 = rng() % 8;
        if (x1 > x2) swap(x1, x2);
        if (y1 > y2) swap(y1, y2);
        if (rng() & 1) {
            long long v = (int)rng() % 21 - 10;
            lazy.add(x1, x2, y1, y2, v);
            for (int i = x1; i < x2; ++i) for (int j = y1; j < y2; ++j) a[i][j] += v;
        } else {
            long long expected = 0;
            for (int i = x1; i < x2; ++i) for (int j = y1; j < y2; ++j) expected += a[i][j];
            assert(lazy.sum(x1, x2, y1, y2) == expected);
        }
    }
}

static void test_optimization_and_strings() {
    ConvexHullTrick<long long> cht;
    vector<pair<long long, long long>> lines{{5, -20}, {2, -5}, {-1, 2}, {-3, 4}};
    for (auto [a, b] : lines) cht.add_line(a, b);
    for (long long x = -10; x <= 10; ++x) {
        long long expected = LLONG_MAX;
        for (auto [a, b] : lines) expected = min(expected, a * x + b);
        assert(cht.query(x) == expected);
    }
    string s = "ababa";
    assert((z_algorithm(s) == vector<int>{5, 0, 3, 0, 1}));
    vector<long long> fib{0, 1}, coef{1, 1};
    assert(kitamasa<long long>(0, fib, coef) == 0);
    assert(kitamasa<long long>(10, fib, coef) == 55);
}

static void test_graphs() {
    Dijkstra<long long> d(4);
    d.add_edge(0, 1, 3); d.add_edge(0, 2, 10); d.add_edge(1, 2, 4); d.add_edge(2, 3, 2);
    auto dist = d.shortest_path(0, (long long)4e18);
    assert((dist == vector<long long>{0, 3, 7, 9}));
    KruskalMST<long long> mst(4);
    mst.add_edge(0, 1, 1); mst.add_edge(1, 2, 5); mst.add_edge(0, 2, 2); mst.add_edge(2, 3, 4);
    auto [cost, edges] = mst.build();
    assert(cost == 7 && edges.size() == 3);
}

static void test_math() {
    Matrix<Mint> a{{1, 1}, {1, 0}};
    auto p = matrix_pow(a, 10);
    assert(p[0][1] == Mint(55));
    assert(determinant(Matrix<Mint>{{1, 2}, {3, 4}}) == Mint(-2));
    auto inv = inverse_matrix(Matrix<Mint>{{1, 2}, {3, 4}});
    assert(inv && matrix_mul(Matrix<Mint>{{1, 2}, {3, 4}}, *inv) == matrix_identity<Mint>(2));
    auto rinv = inverse_matrix_rational<long long>({{1, 2}, {3, 4}});
    assert(rinv && (*rinv)[0][0] == Rational<long long>(-2) && (*rinv)[0][1] == Rational<long long>(1));
    assert((*rinv)[1][0] == Rational<long long>(3, 2) && (*rinv)[1][1] == Rational<long long>(-1, 2));
    assert(determinant_bareiss<long long>({{2, 3, 1}, {4, 1, -3}, {1, 2, 0}}) == 10);
    auto mu = mobius_sieve(10);
    assert((vector<int>(mu.begin() + 1, mu.end()) == vector<int>{1, -1, -1, 0, -1, 1, -1, 0, 0, 1}));
    vector<long long> f{0, 1, 2, 3, 4, 5, 6};
    assert(divisor_mobius(divisor_zeta(f)) == f);
    assert(multiple_mobius(multiple_zeta(f)) == f);
    MappingTwelvefold<long long> twelve(20);
    assert(twelve.distinct_to_distinct_any(3, 2) == 8);
    assert(twelve.distinct_to_distinct_surjective(3, 2) == 6);
    assert(twelve.identical_to_distinct_any(4, 3) == 15);
    assert(twelve.identical_to_identical_surjective(5, 2) == 2);
}

int main() {
    test_dynamic_segment_tree(); test_2d_structures(); test_optimization_and_strings();
    test_graphs(); test_math();
    cout << "requested library tests passed\n";
}
