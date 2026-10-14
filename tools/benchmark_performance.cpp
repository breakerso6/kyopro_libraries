// Compile with -std=c++17 -O2 -I<library-root> -I<ACL-root>.
// Inputs are deterministic; only the measured library operations are timed.
#include <bits/stdc++.h>
#include <atcoder/modint>
#include "libraries/algorithm/Kitamasa.hpp"
#include "libraries/string/AhoCorasick.hpp"
#include "libraries/data_structure/LazySegmentTree2D.hpp"
#include "libraries/math/FormalPowerSeries.hpp"
#include "libraries/math/Matrix.hpp"
#include "libraries/geometry/ClosestPair.hpp"
#include "libraries/geometry/RectangleUnionArea.hpp"

using Mint = atcoder::modint998244353;
using FPS = FormalPowerSeries<Mint>;
using Point = geometry_int::Point<long long>;

template<class F>
void measure(const char* name, F operation) {
    std::vector<double> times;
    uint64_t checksum = 0;
    for (int trial = 0; trial < 5; ++trial) {
        auto start = std::chrono::steady_clock::now();
        uint64_t current = operation();
        auto end = std::chrono::steady_clock::now();
        if (trial && current != checksum) std::abort();
        checksum = current;
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
    }
    std::sort(times.begin(), times.end());
    std::cout << name << ',' << times[2] << ',' << checksum << '\n';
}

int main() {
    std::cout << "case,median_ms,checksum\n";
    std::mt19937_64 rng(20261014);
    std::vector<std::vector<long long>> grid(512, std::vector<long long>(512));
    for (auto& row : grid) for (auto& x : row) x = (long long)(rng() % 1000) - 500;
    measure("range_add_sum_2d_build", [&] {
        RangeAddRangeSum2D<long long> tree(grid);
        return (uint64_t)tree.sum(0, 512, 0, 512);
    });
    AhoCorasick<> ac;
    for (int length = 1; length <= 1000; ++length) ac.add(std::string(length, 'a'));
    ac.build();
    std::string text(200000, 'a');
    measure("aho_nested_count", [&] {
        auto counts = ac.count_matches(text);
        return std::accumulate(counts.begin(), counts.end(), uint64_t(0));
    });
    AhoCorasick<> sparse;
    for (int i = 0; i < 10000; ++i) {
        std::string pattern(12, 'a');
        for (char& c : pattern) c += rng() % 26;
        sparse.add(pattern);
    }
    sparse.build();
    std::string random_text(1000000, 'a');
    for (char& c : random_text) c += rng() % 26;
    measure("aho_random_count", [&] {
        auto counts = sparse.count_matches(random_text);
        return std::accumulate(counts.begin(), counts.end(), uint64_t(0));
    });
    std::vector<Mint> coefficients(256), initial(256);
    for (auto& x : coefficients) x = rng();
    for (auto& x : initial) x = rng();
    measure("kitamasa_dense_bits", [&] {
        return kitamasa<Mint>((1LL << 60) - 1, initial, coefficients).val();
    });
    measure("kitamasa_single_bit", [&] {
        return kitamasa<Mint>(1LL << 60, initial, coefficients).val();
    });
    FPS f(200000);
    for (auto& x : f.a) x = rng();
    measure("fps_integral", [&] {
        auto result = f.integral();
        Mint sum = 0;
        for (auto x : result.a) sum += x;
        return sum.val();
    });
    FPS g(16384);
    for (auto& x : g.a) x = rng();
    g.a[0] = 0;
    measure("fps_exp", [&] {
        auto result = g.exp(g.size());
        Mint sum = 0;
        for (auto x : result.a) sum += x;
        return sum.val();
    });
    std::vector<Point> points(200000);
    for (auto& p : points) p = {(long long)(rng() % 1000000000), (long long)(rng() % 1000000000)};
    measure("closest_pair_random", [&] { return (uint64_t)geometry_int::closest_pair_distance2(points); });
    for (int i = 0; i < (int)points.size(); ++i) points[i] = {i, 0};
    measure("closest_pair_collinear", [&] { return (uint64_t)geometry_int::closest_pair_distance2(points); });
    points.back() = points.front();
    measure("closest_pair_duplicate", [&] { return (uint64_t)geometry_int::closest_pair_distance2(points); });
    std::vector<std::array<long long, 4>> rectangles(100000);
    for (auto& r : rectangles) {
        long long x = rng() % 1000000, y = rng() % 1000000;
        r = {x, y, x + 1 + (long long)(rng() % 1000), y + 1 + (long long)(rng() % 1000)};
    }
    measure("rectangle_union_random", [&] { return (uint64_t)rectangle_union_area(rectangles); });
    for (int i = 0; i < (int)rectangles.size(); ++i) rectangles[i] = {-i - 1, -i - 1, i + 1, i + 1};
    measure("rectangle_union_nested", [&] { return (uint64_t)rectangle_union_area(rectangles); });
    Matrix<Mint> matrix(192, std::vector<Mint>(192));
    for (auto& row : matrix) for (auto& x : row) x = rng();
    measure("matrix_pow_one", [&] {
        auto result = matrix_pow(matrix, 1);
        Mint sum = 0;
        for (auto& row : result) for (auto x : row) sum += x;
        return sum.val();
    });
}
