#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include <bits/stdc++.h>
#include <atcoder/modint>
#include "libraries/algorithm/Kitamasa.hpp"
#include "libraries/string/AhoCorasick.hpp"
#include "libraries/data_structure/LazySegmentTree2D.hpp"
#include "libraries/math/FormalPowerSeries.hpp"
#include "libraries/math/Matrix.hpp"
#include "libraries/geometry/ClosestPair.hpp"
#include "libraries/geometry/RectangleUnionArea.hpp"

using namespace std;
using Mint = atcoder::modint998244353;

static void test_recurrences() {
    mt19937 rng(123456);
    for (int k = 1; k <= 16; ++k) for (int trial = 0; trial < 20; ++trial) {
        vector<Mint> initial(k), c(k), sequence(180);
        for (int i = 0; i < k; ++i) sequence[i] = initial[i] = rng(), c[i] = rng();
        for (int n = k; n < (int)sequence.size(); ++n)
            for (int j = 0; j < k; ++j) sequence[n] += c[j] * sequence[n - k + j];
        for (int n = 0; n < (int)sequence.size(); ++n)
            assert(kitamasa<Mint>(n, initial, c) == sequence[n]);
    }
    // Exercise the highest supported bit without signed shifts overflowing.
    vector<Mint> c{Mint(7)}, initial{Mint(3)};
    assert(kitamasa<Mint>(LLONG_MAX, initial, c) == initial[0] * c[0].pow(LLONG_MAX));
    Matrix<Mint> matrix{{2, 3}, {5, 7}}, expected = matrix_identity<Mint>(2);
    for (int n = 0; n < 80; ++n) {
        assert(matrix_pow(matrix, n) == expected);
        expected = matrix_mul(expected, matrix);
    }
}

static void test_matching() {
    mt19937 rng(4321);
    for (int trial = 0; trial < 300; ++trial) {
        AhoCorasick<3> ac;
        vector<string> patterns;
        for (int i = 0; i < 25; ++i) {
            string s(rng() % 8, 'a');
            for (char& c : s) c += rng() % 3;
            patterns.push_back(s);
            assert(ac.add(s) == i);
        }
        ac.build();
        for (int repeat = 0; repeat < 3; ++repeat) {
            string text(rng() % 120, 'a');
            for (char& c : text) c += rng() % 3;
            auto result = ac.count_matches(text);
            for (int id = 0; id < (int)patterns.size(); ++id) {
                int expected = 0;
                // Preserve existing semantics: empty patterns match once
                // per processed character, excluding the initial boundary.
                if (patterns[id].empty()) expected = text.size();
                else for (size_t i = 0; i + patterns[id].size() <= text.size(); ++i)
                    expected += text.compare(i, patterns[id].size(), patterns[id]) == 0;
                assert(result[id] == expected);
            }
        }
    }
    AhoCorasick<> nested;
    for (int i = 1; i <= 1000; ++i) nested.add(string(i, 'a'));
    nested.build();
    auto short_counts = nested.count_matches("a");
    assert(short_counts[0] == 1);
    for (int i = 1; i < 1000; ++i) assert(short_counts[i] == 0);
    assert(nested.count_matches("") == vector<int>(1000));
    auto counts = nested.count_matches(string(100000, 'a'));
    for (int i = 0; i < 1000; ++i) assert(counts[i] == 100000 - i);
}

template<class M>
static void test_integral() {
    mt19937 rng(333);
    for (int n : {0, 1, 2, 3, 61, 127, 1000}) {
        FormalPowerSeries<M> f(n);
        for (auto& x : f.a) x = rng();
        auto result = f.integral();
        assert(result.size() == n + 1 && result.a[0] == M(0));
        for (int i = 0; i < n; ++i) assert(result.a[i + 1] == f.a[i] / M(i + 1));
        assert(result.diff() == f);
    }
}

static void test_geometry() {
    using Point = geometry_int::Point<long long>;
    using I128 = __int128_t;
    assert(geometry_int::closest_pair_distance2(vector<Point>{}) == -1);
    vector<Point> points;
    for (int i = 0; i < 10000; ++i) points.push_back({0, (long long)i * 1000000000});
    assert(geometry_int::closest_pair_distance2(points) == I128(1000000000) * 1000000000);
    points.push_back(points[1234]);
    assert(geometry_int::closest_pair_distance2(points) == 0);
    vector<array<long long, 4>> rectangles;
    for (int i = 1; i <= 10000; ++i) rectangles.push_back({-i, -i, i, i});
    assert(rectangle_union_area(rectangles) == I128(20000) * 20000);
    rectangles = {{-4000000000LL, -4000000000LL, 4000000000LL, 4000000000LL}};
    assert(rectangle_union_area(rectangles) == I128(8000000000LL) * 8000000000LL);
}

static void test_grid_build() {
    mt19937 rng(789);
    for (int height : {0, 1, 2, 7, 16}) for (int width : {0, 1, 3, 8, 17}) {
        vector<vector<long long>> a(height, vector<long long>(width));
        for (auto& row : a) for (auto& x : row) x = (int)(rng() % 101) - 50;
        RangeAddRangeSum2D<long long> tree(a);
        for (int trial = 0; trial < 300; ++trial) {
            int l = rng() % (height + 1), r = rng() % (height + 1);
            int actual_width = height ? width : 0;
            int d = rng() % (actual_width + 1), u = rng() % (actual_width + 1);
            if (l > r) swap(l, r);
            if (d > u) swap(d, u);
            if (trial & 1) {
                long long value = (int)(rng() % 101) - 50;
                tree.add(l, r, d, u, value);
                for (int i = l; i < r; ++i) for (int j = d; j < u; ++j) a[i][j] += value;
            } else {
                long long expected = 0;
                for (int i = l; i < r; ++i) for (int j = d; j < u; ++j) expected += a[i][j];
                assert(tree.sum(l, r, d, u) == expected);
            }
        }
    }
}

int main() {
    test_recurrences();
    test_matching();
    test_integral<Mint>();
    test_integral<Rational<long long>>();
    FormalPowerSeries<double, false> floating(1000);
    for (int i = 0; i < floating.size(); ++i) floating.a[i] = i + 1;
    auto floating_integral = floating.integral();
    for (int i = 1; i < floating_integral.size(); ++i) assert(floating_integral.a[i] == 1.0);
    atcoder::dynamic_modint<42>::set_mod(1000000007);
    test_integral<atcoder::dynamic_modint<42>>();
    // Batch inversion also works when the modulus is composite and all
    // required divisors are units.
    atcoder::dynamic_modint<42>::set_mod(35);
    FormalPowerSeries<atcoder::dynamic_modint<42>> f{1, 2, 3, 4};
    assert(f.integral().diff() == f);
    test_geometry();
    test_grid_build();
    cout << "Hello World\n";
}
