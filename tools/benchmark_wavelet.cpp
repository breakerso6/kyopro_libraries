// Compile with -std=c++17 -O2 -I<library-root>.
#include <bits/stdc++.h>
#include "libraries/data_structure/WaveletMatrix.hpp"
#include "libraries/algorithm/Mo.hpp"
#include "libraries/math/Factorization.hpp"

template<class F>
void measure(const char* name, F operation) {
    std::vector<double> times;
    uint64_t checksum = 0;
    for (int trial = 0; trial < 5; ++trial) {
        auto begin = std::chrono::steady_clock::now();
        uint64_t value = operation();
        auto end = std::chrono::steady_clock::now();
        if (trial && value != checksum) std::abort();
        checksum = value;
        times.push_back(std::chrono::duration<double, std::milli>(end - begin).count());
    }
    std::sort(times.begin(), times.end());
    std::cout << name << ',' << times[2] << ',' << checksum << std::endl;
}

int main(int argc, char** argv) {
    std::mt19937_64 rng(20261015);
    const int n = 200000, queries = 50000;
    std::vector<int> key(n), small_key(n), x(n);
    std::vector<long long> weight(n);
    for (int i = 0; i < n; ++i) {
        key[i] = rng() % 100000;
        small_key[i] = rng() % 4;
        x[i] = rng() % 1000000;
        weight[i] = rng() % 1000;
    }
    // Run one construction in a fresh process for peak-RSS measurement.
    if (argc == 2 && std::string(argv[1]) == "memory-group") {
        WaveletMatrixSum<int, long long> tree(key, weight);
        std::cout << tree.all_prod(0, n) << '\n';
        return 0;
    }
    if (argc == 2 && std::string(argv[1]) == "memory-monoid") {
        DynamicWaveletMatrixSum<int, long long> tree(key, weight);
        std::cout << tree.all_prod(0, n) << '\n';
        return 0;
    }
    std::cout << "case,median_ms,checksum\n";
    measure("wm_build_large_alphabet", [&] {
        WaveletMatrix<int> matrix(key);
        return matrix.alphabet_size() + matrix.kth_smallest(0, n, n / 2);
    });
    measure("wm_build_small_alphabet", [&] {
        WaveletMatrix<int> matrix(small_key);
        return matrix.alphabet_size() + matrix.kth_smallest(0, n, n / 2);
    });
    WaveletMatrix<int> matrix(key), small_matrix(small_key);
    std::vector<int> index(500000);
    for (auto& i : index) i = rng() % n;
    measure("wm_access", [&] {
        uint64_t checksum = 0;
        for (int i : index) checksum += matrix.access(i);
        return checksum;
    });
    std::vector<std::array<int, 4>> query(queries);
    for (auto& q : query) {
        q = {(int)(rng() % n), (int)(rng() % n), (int)(rng() % 100000), (int)(rng() % 100000)};
        if (q[0] > q[1]) std::swap(q[0], q[1]);
        if (q[2] > q[3]) std::swap(q[2], q[3]);
    }
    measure("wm_select", [&] {
        uint64_t checksum = 0;
        for (auto q : query) checksum += matrix.select(key[q[0]], 0).value_or(-1) + 1;
        return checksum;
    });
    measure("wm_count_sparse", [&] {
        uint64_t checksum = 0;
        for (auto q : query) checksum += matrix.count(q[0], q[1], key[q[0]]);
        return checksum;
    });
    measure("wm_count_dense", [&] {
        uint64_t checksum = 0;
        for (auto q : query) checksum += small_matrix.count(q[0], q[1], small_key[q[0]]);
        return checksum;
    });
    measure("wm_range_freq_narrow", [&] {
        uint64_t checksum = 0;
        for (auto q : query) checksum += matrix.range_freq(q[0], q[1], q[2], q[2] + 3);
        return checksum;
    });
    measure("wm_range_freq_wide", [&] {
        uint64_t checksum = 0;
        for (auto q : query) checksum += matrix.range_freq(q[0], q[1], q[2], q[3]);
        return checksum;
    });
    measure("wm_kth", [&] {
        uint64_t checksum = 0;
        for (auto q : query) if (q[0] < q[1]) checksum += matrix.kth_smallest(q[0], q[1], (q[1] - q[0]) / 2);
        return checksum;
    });
    std::vector<unsigned> uint_key(key.begin(), key.end());
    XorWaveletMatrix<unsigned, 20> xor_matrix(uint_key);
    measure("xor_wm_access", [&] {
        uint64_t checksum = 0;
        for (int i : index) checksum += xor_matrix.access(i);
        return checksum;
    });
    measure("wm_sum_build", [&] {
        WaveletMatrixSum<int, long long> tree(key, weight);
        return (uint64_t)tree.all_prod(0, n);
    });
    WaveletMatrixSum<int, long long> sums(key, weight);
    measure("wm_sum_prod", [&] {
        uint64_t checksum = 0;
        for (auto q : query) checksum += sums.prod(q[0], q[1], q[2], q[3]);
        return checksum;
    });
    measure("wm_dynamic_sum_build", [&] {
        DynamicWaveletMatrixSum<int, long long> tree(key, weight);
        return (uint64_t)tree.all_prod(0, n);
    });
    measure("wm_2d_sum_build", [&] {
        WaveletMatrix2DSum<int, int, long long> tree(x, key, weight);
        return (uint64_t)tree.rectangle_prod(0, 1000000, 0, 100000);
    });
    Mo mo(n);
    for (auto q : query) mo.add_query(q[0], q[1]);
    measure("mo_sort_noop", [&] {
        uint64_t checksum = 0;
        auto noop = [](int) {};
        mo.run(noop, noop, noop, noop, [&](int id) { checksum = checksum * 1000003 + id; });
        return checksum;
    });
    measure("mo_sum", [&] {
        uint64_t checksum = 0;
        long long sum = 0;
        auto add = [&](int i) { sum += weight[i]; };
        auto erase = [&](int i) { sum -= weight[i]; };
        mo.run(add, add, erase, erase, [&](int) { checksum += sum; });
        return checksum;
    });
    std::vector<uint64_t> semiprimes;
    // Generate deterministic prime inputs outside the measured region.
    for (uint64_t candidate = 4000000001ULL; semiprimes.size() < 20; candidate += 2)
        if (factorization::is_prime(candidate)) semiprimes.push_back(candidate);
    measure("factorization_semiprimes", [&] {
        uint64_t checksum = 0;
        for (int i = 0; i < 10; ++i) {
            auto factors = factorization::prime_factors(semiprimes[i] * semiprimes[i + 10]);
            for (auto p : factors) checksum += p;
        }
        return checksum;
    });
}
