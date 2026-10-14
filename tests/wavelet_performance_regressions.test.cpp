#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include <bits/stdc++.h>
#include "libraries/data_structure/WaveletMatrix.hpp"
#include "libraries/math/Factorization.hpp"
#include "libraries/algorithm/Mo.hpp"

using namespace std;

struct LessOnly {
    int value;
    bool operator<(const LessOnly& other) const { return value < other.value; }
};

unsigned xor_op(unsigned a, unsigned b) { return a ^ b; }
unsigned xor_e() { return 0; }
unsigned xor_inverse(unsigned a) { return a; }

static void test_wavelet_rebuild() {
    mt19937 rng(1717);
    WaveletMatrix<int> matrix;
    for (int n : {0, 1, 2, 7, 64, 150, 3, 0}) for (int sigma : {1, 2, 3, 16, 256}) {
        vector<int> a(n);
        for (auto& value : a) value = (int)(rng() % sigma) - sigma / 2;
        matrix.build(a);
        for (int i = 0; i < n; ++i) assert(matrix.access(i) == a[i]);
        for (int value = -sigma / 2 - 1; value <= sigma / 2 + 1; ++value) {
            vector<int> indices;
            for (int i = 0; i < n; ++i) if (a[i] == value) indices.push_back(i);
            assert(!matrix.select(value, -1));
            for (int k = 0; k < (int)indices.size(); ++k) assert(matrix.select(value, k) == optional<int>(indices[k]));
            assert(!matrix.select(value, indices.size()));
            assert(!matrix.select(value, INT_MAX));
        }
        for (int query = 0; query < 500; ++query) {
            int l = rng() % (n + 1), r = rng() % (n + 1);
            if (l > r) swap(l, r);
            int low = (int)(rng() % (sigma + 4)) - sigma / 2 - 2;
            int high = (int)(rng() % (sigma + 4)) - sigma / 2 - 2;
            if (low > high) swap(low, high);
            int expected = 0;
            for (int i = l; i < r; ++i) expected += low <= a[i] && a[i] < high;
            assert(matrix.range_freq(l, r, low, high) == expected);
            assert(matrix.count(l, r, low) == (int)count(a.begin() + l, a.begin() + r, low));
            auto segments = matrix.range_segments(l, r, low, high);
            size_t current = 0;
            matrix.for_each_range_segment(l, r, low, high, [&](auto segment) {
                assert(current < segments.size());
                auto reference = segments[current++];
                assert(segment.depth == reference.depth && segment.left == reference.left && segment.right == reference.right);
            });
            assert(current == segments.size());
        }
        vector<int> indices(n);
        iota(indices.begin(), indices.end(), 0);
        auto reordered = matrix.reorder_each_level(indices);
        matrix.for_each_reordered_level(indices, [&](int depth, const vector<int>& current) {
            assert(current == indices);
            assert(reordered[depth] == indices);
            if (depth < matrix.levels) {
                int bit = matrix.levels - depth - 1;
                stable_partition(indices.begin(), indices.end(), [&](int i) { return ((matrix.original_code[i] >> bit) & 1) == 0; });
            }
        });
    }
    WaveletMatrix<LessOnly> custom(vector<LessOnly>{{2}, {1}, {2}, {3}});
    assert(custom.select({2}, 1) == optional<int>(2));
    assert(custom.count(1, 4, {2}) == 1);
    assert(custom.range_freq(0, 4, {2}, {3}) == 2);
    // build may receive the object's own values as the source.
    matrix.build(vector<int>{3, 1, 3, 2});
    matrix.build(matrix.values);
    assert(matrix.size() == 3 && matrix[0] == 1 && matrix[2] == 3);
    WaveletMatrix<long long> signed_limits(vector<long long>{LLONG_MIN, LLONG_MIN + 1, 0, LLONG_MAX});
    assert(signed_limits.range_freq(0, 4, LLONG_MIN, LLONG_MIN + 2) == 2);
    assert(signed_limits.range_freq(0, 4, LLONG_MIN, LLONG_MAX) == 3);
    WaveletMatrix<uint64_t> unsigned_limits(vector<uint64_t>{0, UINT64_MAX - 2, UINT64_MAX - 1, UINT64_MAX});
    assert(unsigned_limits.range_freq(1, 4, UINT64_MAX - 2, UINT64_MAX) == 2);
    assert(unsigned_limits.range_freq(0, 4, 0, UINT64_MAX) == 3);
    XorWaveletMatrix<uint64_t> wide(vector<uint64_t>{0, UINT64_MAX, 1ULL << 63});
    assert(wide.access(1) == UINT64_MAX);
    assert(wide.kth_smallest(0, 3, 0, UINT64_MAX) == 0);
    wide.build(vector<uint64_t>{});
    assert(wide.empty());
    wide.build(vector<uint64_t>{7});
    assert(wide.access(0) == 7);
}

static void test_shared_layout() {
    vector<int> key{4, 1, 4, 7, 3};
    vector<long long> weight{5, -2, 9, 3, 7};
    WaveletMatrix<int> layout(key);
    WaveletMatrixSum<int, long long> group;
    DynamicWaveletMatrixSum<int, long long> monoid;
    group.build(layout, weight);
    monoid.build(layout, weight);
    // The layouts are owned copies, independent of later changes to source.
    layout.build(vector<int>{});
    for (int l = 0; l <= 5; ++l) for (int r = l; r <= 5; ++r)
        for (int low = 0; low < 9; ++low) for (int high = low; high < 9; ++high) {
            long long sum = 0;
            for (int i = l; i < r; ++i) if (low <= key[i] && key[i] < high) sum += weight[i];
            assert(group.prod(l, r, low, high) == sum);
            assert(monoid.prod(l, r, low, high) == sum);
        }
    monoid.set(2, 100);
    assert(monoid.prod(0, 5, 4, 5) == 105);
    group.build(vector<int>{}, vector<long long>{});
    monoid.build(vector<int>{}, vector<long long>{});
    assert(group.all_prod(0, 0) == 0 && monoid.all_prod(0, 0) == 0);
    vector<unsigned> bits{3, 5, 7, 9, 11};
    WaveletMatrixGroup<int, unsigned, xor_op, xor_e, xor_inverse> xor_group(key, bits);
    for (int low = 0; low <= 8; ++low) for (int high = low; high <= 8; ++high) {
        unsigned expected = 0;
        for (int i = 0; i < 5; ++i) if (low <= key[i] && key[i] < high) expected ^= bits[i];
        assert(xor_group.prod(0, 5, low, high) == expected);
    }
}

static void test_factorization() {
    using factorization::u64;
    mt19937_64 rng(3456);
    for (int trial = 0; trial < 1000; ++trial) {
        u64 n = 1 + rng() % 100000, remaining = n;
        vector<u64> expected;
        for (u64 p = 2; p * p <= remaining; ++p)
            while (remaining % p == 0) { expected.push_back(p); remaining /= p; }
        if (remaining > 1) expected.push_back(remaining);
        assert(factorization::prime_factors(n) == expected);
    }
    for (u64 n : std::initializer_list<u64>{1ULL, 49ULL, 121ULL, 3486784401ULL, 1000000016000000063ULL,
                  18446744030759878681ULL, 18446744073709551557ULL, UINT64_MAX}) {
        auto factors = factorization::prime_factors(n);
        __uint128_t product = 1;
        for (u64 p : factors) {
            assert(factorization::is_prime(p));
            product *= p;
        }
        assert(product == n && is_sorted(factors.begin(), factors.end()));
    }
    for (int trial = 0; trial < 100; ++trial)
        assert((factorization::prime_factors(49) == vector<u64>{7, 7}));
}

static void test_mo() {
    mt19937 rng(8899);
    for (int n : {0, 1, 17, 100}) {
        vector<long long> a(n), prefix(n + 1);
        for (int i = 0; i < n; ++i) prefix[i + 1] = prefix[i] + (a[i] = (int)(rng() % 101) - 50);
        Mo mo(n);
        vector<long long> expected;
        for (int q = 0; q < 1000; ++q) {
            int l = rng() % (n + 1), r = rng() % (n + 1);
            if (l > r) swap(l, r);
            assert(mo.add_query(l, r) == q);
            expected.push_back(prefix[r] - prefix[l]);
        }
        for (int repeat = 0; repeat < 2; ++repeat) {
            long long sum = 0;
            vector<char> seen(expected.size());
            auto add = [&](int i) { sum += a[i]; };
            auto erase = [&](int i) { sum -= a[i]; };
            mo.run(add, add, erase, erase, [&](int id) {
                assert(!seen[id] && sum == expected[id]);
                seen[id] = true;
            });
            assert(count(seen.begin(), seen.end(), true) == (int)expected.size());
        }
    }
    Mo empty(5);
    auto noop = [](int) { assert(false); };
    empty.run(noop, noop, noop, noop, noop);
}

int main() {
    test_wavelet_rebuild();
    test_shared_layout();
    test_factorization();
    test_mo();
    cout << "Hello World\n";
}
