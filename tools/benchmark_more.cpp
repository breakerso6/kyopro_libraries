// Compile with -std=c++17 -O2 -I<library-root>.
#include <bits/stdc++.h>
#include "libraries/algorithm/SMAWK.hpp"
#include "libraries/data_structure/LiChaoTree.hpp"
#include "libraries/data_structure/PersistentSegmentTree.hpp"
#include "libraries/data_structure/DynamicSegmentTree.hpp"
#include "libraries/graph/HopcroftKarp.hpp"

long long sum_op(long long a, long long b) { return a + b; }
long long sum_e() { return 0; }

template<class F>
void measure(const std::string& name, F operation) {
    std::vector<double> times;
    uint64_t checksum = 0;
    for (int trial = 0; trial < 5; ++trial) {
        auto begin = std::chrono::steady_clock::now();
        uint64_t current = operation();
        auto end = std::chrono::steady_clock::now();
        if (trial && current != checksum) std::abort();
        checksum = current;
        times.push_back(std::chrono::duration<double, std::milli>(end - begin).count());
    }
    std::sort(times.begin(), times.end());
    std::cout << name << ',' << times[2] << ',' << checksum << std::endl;
}

int main() {
    std::cout << "case,median_ms,checksum\n";
    for (int n : {6000, 12000, 24000}) {
        measure("smawk_diagonal_" + std::to_string(n), [&] {
            auto minima = smawk(n, n, [](int row, int column) {
                long long delta = row - column;
                return delta * delta;
            });
            return std::accumulate(minima.begin(), minima.end(), uint64_t(0));
        });
    }
    measure("smawk_ties", [&] {
        auto minima = smawk(24000, 24000, [](int, int) { return 0; });
        return std::accumulate(minima.begin(), minima.end(), uint64_t(0));
    });
    std::mt19937_64 rng(7654321);
    const int n = 200000, queries = 100000;
    std::vector<std::pair<long long, long long>> lines(n);
    for (auto& line : lines) line = {(long long)(rng() % 20001) - 10000, (long long)(rng() % 2000000001) - 1000000000};
    const long long inf = 4000000000000000000LL;
    measure("li_chao_random_add", [&] {
        LiChaoTree<long long> tree(-1000000000, 1000000000, inf);
        for (auto [a, b] : lines) tree.add_line(a, b);
        return (uint64_t)tree.min(123456789);
    });
    measure("li_chao_equal_slope_add", [&] {
        LiChaoTree<long long> tree(-1000000000, 1000000000, inf);
        for (int i = 0; i < n; ++i) tree.add_line(100, i);
        return (uint64_t)tree.min(123456789);
    });
    measure("li_chao_active_add", [&] {
        LiChaoTree<long long> tree(0, n, inf);
        for (long long i = 0; i < n; ++i) tree.add_line(-2 * i, i * i);
        return (uint64_t)tree.min(n / 2);
    });
    LiChaoTree<long long> hull(-1000000000, 1000000000, inf);
    for (auto [a, b] : lines) hull.add_line(a, b);
    std::vector<long long> xs(queries);
    for (auto& x : xs) x = (long long)(rng() % 2000000000) - 1000000000;
    measure("li_chao_query", [&] {
        uint64_t checksum = 0;
        for (long long x : xs) checksum += hull.min(x);
        return checksum;
    });
    std::vector<long long> values(n);
    for (auto& x : values) x = rng() % 1000;
    measure("persistent_seg_build", [&] {
        PersistentSegmentTree<long long, sum_op, sum_e> tree(n);
        int root = tree.build(values);
        return (uint64_t)tree.prod(root, 0, n);
    });
    PersistentSegmentTree<long long, sum_op, sum_e> persistent(n);
    int root = persistent.build(values);
    std::vector<std::pair<int, int>> ranges(queries);
    for (auto& range : ranges) {
        range = {(int)(rng() % n), (int)(rng() % n)};
        if (range.first > range.second) std::swap(range.first, range.second);
    }
    measure("persistent_seg_get", [&] {
        uint64_t checksum = 0;
        for (auto [l, r] : ranges) { (void)r; checksum += persistent.get(root, l); }
        return checksum;
    });
    measure("persistent_seg_prod", [&] {
        uint64_t checksum = 0;
        for (auto [l, r] : ranges) checksum += persistent.prod(root, l, r);
        return checksum;
    });
    DynamicSegmentTree<long long, sum_op, sum_e> dynamic(0, n);
    for (int i = 0; i < n; ++i) dynamic.set(i, values[i]);
    measure("dynamic_seg_get", [&] {
        uint64_t checksum = 0;
        for (auto [l, r] : ranges) { (void)r; checksum += dynamic.get(l); }
        return checksum;
    });
    measure("dynamic_seg_prod", [&] {
        uint64_t checksum = 0;
        for (auto [l, r] : ranges) checksum += dynamic.prod(l, r);
        return checksum;
    });
    HopcroftKarp dense(2000, 2000);
    for (int i = 0; i < 2000; ++i) for (int j = 0; j < 2000; ++j) dense.add_edge(i, j);
    measure("hopcroft_dense", [&] {
        std::fill(dense.match_left.begin(), dense.match_left.end(), -1);
        std::fill(dense.match_right.begin(), dense.match_right.end(), -1);
        return dense.max_matching();
    });
    HopcroftKarp sparse(30000, 30000);
    for (int i = 0; i < 30000; ++i) {
        for (int j = 0; j < 7; ++j) sparse.add_edge(i, rng() % 30000);
        sparse.add_edge(i, i);
    }
    measure("hopcroft_sparse", [&] {
        std::fill(sparse.match_left.begin(), sparse.match_left.end(), -1);
        std::fill(sparse.match_right.begin(), sparse.match_right.end(), -1);
        return sparse.max_matching();
    });
    HopcroftKarp unbalanced(30000, 4);
    for (int i = 0; i < 30000; ++i) for (int j = 0; j < 4; ++j) unbalanced.add_edge(i, j);
    measure("hopcroft_unbalanced", [&] {
        std::fill(unbalanced.match_left.begin(), unbalanced.match_left.end(), -1);
        std::fill(unbalanced.match_right.begin(), unbalanced.match_right.end(), -1);
        return unbalanced.max_matching();
    });
}
