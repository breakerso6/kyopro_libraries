#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include <bits/stdc++.h>
using namespace std;

#include "libraries/data_structure/WaveletMatrix.hpp"

long long sum_op(long long a, long long b) { return a + b; }
long long sum_e() { return 0; }
long long sum_inv(long long a) { return -a; }
long long max_op(long long a, long long b) { return max(a, b); }
long long max_e() { return numeric_limits<long long>::lowest(); }

struct LessOnly {
    int value;
    bool operator<(const LessOnly& other) const { return value < other.value; }
};

static void test_basic_extensions() {
    vector<int> a = {4, -1, 4, 7, 0, -1};
    WaveletMatrix<int> wm(a);
    assert(wm.size() == 6 && wm.alphabet_size() == 4 && !wm.empty());
    for (int i = 0; i < (int)a.size(); ++i) assert(wm[i] == a[i]);
    assert(wm.count_less_equal(0, 6, 4) == 5);
    assert(wm.range_freq(1, 5, 4) == 2);
    assert(wm.rank(-1, 6) == 2 && wm.rank(-1, 1, 6) == 2);
    assert(wm.select(-1, 0) == optional<int>(1));
    assert(wm.select(-1, 1) == optional<int>(5));
    assert(!wm.select(-1, 2));
    assert(wm.range_min(1, 5) == optional<int>(-1));
    assert(wm.range_max(1, 5) == optional<int>(7));
    assert(!wm.range_min(2, 2));

    vector<long long> payload = {2, 3, 5, 7, 11, 13};
    auto reordered = wm.reorder_each_level(payload);
    for (int depth = 0; depth <= wm.levels; ++depth) {
        auto sorted = reordered[depth];
        sort(sorted.begin(), sorted.end());
        auto expected = payload;
        sort(expected.begin(), expected.end());
        assert(sorted == expected);
    }

    WaveletMatrix<int> empty(vector<int>{});
    assert(empty.empty() && empty.alphabet_size() == 0);
    assert(empty.count_less(0, 0, 10) == 0);
    assert(empty.range_segments(0, 0, -1, 1).empty());

    WaveletMatrix<LessOnly> less_only(vector<LessOnly>{{3}, {1}, {3}});
    assert(less_only.alphabet_size() == 2);
    assert(less_only.access(1).value == 1);
}

static void test_xor_wavelet_matrix() {
    mt19937 rng(20260719);
    for (int iteration = 0; iteration < 30; ++iteration) {
        int n = 1 + rng() % 45;
        vector<unsigned> a(n);
        for (auto& value : a) value = rng() & 255U;
        XorWaveletMatrix<unsigned, 8> wm(a);
        for (int i = 0; i < n; ++i) assert(wm.access(i) == a[i]);
        for (int query = 0; query < 500; ++query) {
            int l = rng() % n, r = rng() % n;
            if (l > r) swap(l, r);
            ++r;
            unsigned mask = rng() & 255U;
            vector<unsigned> transformed;
            for (int i = l; i < r; ++i) transformed.push_back(a[i] ^ mask);
            sort(transformed.begin(), transformed.end());
            int k = rng() % transformed.size();
            assert(wm.kth_smallest(l, r, k, mask) == transformed[k]);
            assert(wm.kth_largest(l, r, k, mask) == transformed[transformed.size() - 1 - k]);
            assert(wm.min_xor(l, r, mask) == transformed.front());
            assert(wm.max_xor(l, r, mask) == transformed.back());
            unsigned lower = rng() % 257, upper = rng() % 257;
            if (lower > upper) swap(lower, upper);
            int less = count_if(transformed.begin(), transformed.end(), [&](unsigned x) { return x < upper; });
            int freq = count_if(transformed.begin(), transformed.end(),
                                [&](unsigned x) { return lower <= x && x < upper; });
            assert(wm.count_less(l, r, upper, mask) == less);
            assert(wm.range_freq(l, r, lower, upper, mask) == freq);
            unsigned value = rng() & 255U;
            assert(wm.count(l, r, value, mask) == (int)count(transformed.begin(), transformed.end(), value));
        }
    }
}

static void test_group_aggregation() {
    mt19937 rng(31415926);
    int n = 70;
    vector<int> key(n);
    vector<long long> weight(n);
    for (int i = 0; i < n; ++i) {
        key[i] = (int)(rng() % 25) - 12;
        weight[i] = rng() % 20;
    }
    WaveletMatrixGroup<int, long long, sum_op, sum_e, sum_inv> wm(key, weight);
    WaveletMatrixSum<int, long long> sum_alias(key, weight);
    for (int query = 0; query < 5000; ++query) {
        int l = rng() % (n + 1), r = rng() % (n + 1);
        if (l > r) swap(l, r);
        int low = (int)(rng() % 31) - 15, high = (int)(rng() % 31) - 15;
        if (low > high) swap(low, high);
        long long expected = 0, expected_less = 0, expected_all = 0;
        for (int i = l; i < r; ++i) {
            if (low <= key[i] && key[i] < high) expected += weight[i];
            if (key[i] < high) expected_less += weight[i];
            expected_all += weight[i];
        }
        assert(wm.prod(l, r, low, high) == expected);
        assert(sum_alias.prod(l, r, low, high) == expected);
        assert(wm.prod_less(l, r, high) == expected_less);
        assert(wm.all_prod(l, r) == expected_all);

        long long budget = rng() % 300;
        auto result = wm.max_prefix_fold(l, r, [&](long long sum) { return sum <= budget; });
        long long accepted = 0;
        int distinct_count = 0;
        for (int value : wm.matrix.values) {
            long long bucket = 0;
            for (int i = l; i < r; ++i) if (key[i] == value) bucket += weight[i];
            if (accepted + bucket > budget) break;
            accepted += bucket;
            ++distinct_count;
        }
        assert(result.distinct_count == distinct_count);
        assert(result.fold == accepted);
    }
}

static void test_dynamic_monoid() {
    mt19937 rng(27182818);
    int n = 55;
    vector<int> key(n);
    vector<long long> weight(n);
    for (int i = 0; i < n; ++i) {
        key[i] = (int)(rng() % 19) - 9;
        weight[i] = (int)(rng() % 101) - 50;
    }
    WaveletMatrixMonoid<int, long long, max_op, max_e> wm(key, weight);
    WaveletMatrixMax<int, long long> max_alias(key, weight);
    for (int step = 0; step < 4000; ++step) {
        if (rng() % 4 == 0) {
            int index = rng() % n;
            long long value = (int)(rng() % 101) - 50;
            weight[index] = value;
            wm.set(index, value);
            max_alias.set(index, value);
            assert(wm.get(index) == value);
        } else {
            int l = rng() % (n + 1), r = rng() % (n + 1);
            if (l > r) swap(l, r);
            int low = (int)(rng() % 25) - 12, high = (int)(rng() % 25) - 12;
            if (low > high) swap(low, high);
            long long expected = max_e();
            for (int i = l; i < r; ++i)
                if (low <= key[i] && key[i] < high) expected = max(expected, weight[i]);
            assert(wm.prod(l, r, low, high) == expected);
            assert(max_alias.prod(l, r, low, high) == expected);
        }
    }

    vector<long long> nonnegative(n);
    for (auto& value : nonnegative) value = rng() % 10;
    DynamicWaveletMatrixSum<int, long long> sum_wm(key, nonnegative);
    for (int step = 0; step < 1000; ++step) {
        if (rng() % 3 == 0) {
            int index = rng() % n;
            nonnegative[index] = rng() % 10;
            sum_wm.set(index, nonnegative[index]);
        }
        int l = rng() % (n + 1), r = rng() % (n + 1);
        if (l > r) swap(l, r);
        long long budget = rng() % 150;
        auto result = sum_wm.max_prefix_fold(l, r, [&](long long sum) { return sum <= budget; });
        long long accepted = 0;
        int distinct_count = 0;
        for (int value : sum_wm.matrix.values) {
            long long bucket = 0;
            for (int i = l; i < r; ++i) if (key[i] == value) bucket += nonnegative[i];
            if (accepted + bucket > budget) break;
            accepted += bucket;
            ++distinct_count;
        }
        assert(result.distinct_count == distinct_count && result.fold == accepted);
    }
}

static void test_two_dimensional() {
    mt19937 rng(16180339);
    int n = 80;
    vector<int> x(n), y(n);
    vector<long long> weight(n);
    for (int i = 0; i < n; ++i) {
        x[i] = (int)(rng() % 31) - 15;
        y[i] = (int)(rng() % 31) - 15;
        weight[i] = (int)(rng() % 101) - 50;
    }
    WaveletMatrix2D<int, int> wm(x, y);
    vector<long long> initial_weight = weight;
    WaveletMatrix2DSum<int, int, long long> sum_wm(x, y, weight);
    WaveletMatrix2DMax<int, int, long long> max_wm(x, y, weight);
    for (int query = 0; query < 4000; ++query) {
        if (rng() % 5 == 0) {
            int index = rng() % n;
            weight[index] = (int)(rng() % 101) - 50;
            max_wm.set(index, weight[index]);
            assert(max_wm.get(index) == weight[index]);
        }
        int left = (int)(rng() % 35) - 17, right = (int)(rng() % 35) - 17;
        int down = (int)(rng() % 35) - 17, up = (int)(rng() % 35) - 17;
        if (left > right) swap(left, right);
        if (down > up) swap(down, up);
        vector<int> ys;
        long long expected_initial_sum = 0, expected_max = max_e();
        for (int i = 0; i < n; ++i)
            if (left <= x[i] && x[i] < right && down <= y[i] && y[i] < up) {
                ys.push_back(y[i]);
                expected_max = max(expected_max, weight[i]);
            }
        // sum_wm is intentionally static, so compare with its construction-time payload.
        for (int sorted = 0; sorted < sum_wm.points.size(); ++sorted) {
            int i = sum_wm.points.sorted_index[sorted];
            if (left <= x[i] && x[i] < right && down <= y[i] && y[i] < up)
                expected_initial_sum += initial_weight[i];
        }
        assert(wm.rectangle_freq(left, right, down, up) == (int)ys.size());
        assert(sum_wm.rectangle_prod(left, right, down, up) == expected_initial_sum);
        assert(max_wm.rectangle_prod(left, right, down, up) == expected_max);
        if (!ys.empty()) {
            sort(ys.begin(), ys.end());
            int k = rng() % ys.size();
            assert(wm.kth_smallest_y(left, right, down, up, k) == ys[k]);
        }
    }
}

int main() {
    test_basic_extensions();
    test_xor_wavelet_matrix();
    test_group_aggregation();
    test_dynamic_monoid();
    test_two_dimensional();
    cout << "Hello World\n";
}
