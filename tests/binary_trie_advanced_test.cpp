#include <bits/stdc++.h>
using namespace std;

#include "libraries/data_structure/BinaryTrie.hpp"

long long bt_sum(long long a, long long b) { return a + b; }
long long bt_zero() { return 0; }
long long bt_max(long long a, long long b) { return max(a, b); }
long long bt_lowest() { return numeric_limits<long long>::lowest(); }

static void apply_xor(multiset<unsigned>& values, unsigned mask) {
    multiset<unsigned> transformed;
    for (unsigned value : values) transformed.insert(value ^ mask);
    values.swap(transformed);
}

static void test_multiset_trie() {
    mt19937 rng(20260720);
    BinaryTrie<unsigned, 8> trie;
    trie.reserve(2000);
    multiset<unsigned> values;

    for (int step = 0; step < 12000; ++step) {
        int type = rng() % 9;
        unsigned value = rng() & 255U;
        if (type <= 2) {
            int occurrences = 1 + rng() % 4;
            trie.insert(value, occurrences);
            for (int i = 0; i < occurrences; ++i) values.insert(value);
        } else if (type == 3) {
            auto it = values.find(value);
            bool expected = it != values.end();
            assert(trie.erase(value) == expected);
            if (expected) values.erase(it);
        } else if (type == 4) {
            int occurrences = rng() % 5;
            int expected = min<int>(occurrences, values.count(value));
            assert(trie.erase(value, occurrences) == expected);
            for (int i = 0; i < expected; ++i) values.erase(values.find(value));
        } else if (type == 5) {
            int expected = values.count(value);
            assert(trie.erase_all(value) == expected);
            values.erase(value);
        } else if (type == 6) {
            unsigned mask = rng() & 255U;
            trie.xor_all(mask);
            apply_xor(values, mask);
        }

        assert(trie.size() == (int)values.size());
        set<unsigned> distinct(values.begin(), values.end());
        assert(trie.distinct_size() == (int)distinct.size());
        assert(trie.empty() == values.empty());
        assert(trie.count(value) == (int)values.count(value));
        assert(trie.contains(value) == bool(values.count(value)));

        unsigned query_mask = rng() & 255U;
        vector<unsigned> transformed;
        for (unsigned x : values) transformed.push_back(x ^ query_mask);
        sort(transformed.begin(), transformed.end());
        if (!transformed.empty()) {
            int k = rng() % transformed.size();
            assert(trie.kth_xor(query_mask, k) == transformed[k]);
            vector<unsigned> plain(values.begin(), values.end());
            assert(trie.kth_smallest(k) == plain[k]);
            assert(trie.kth_largest(k) == plain[plain.size() - 1 - k]);
            assert(trie.min_xor(query_mask) == transformed.front());
            assert(trie.max_xor(query_mask) == transformed.back());
            assert(values.count(trie.argmin_xor(query_mask)));
            assert((trie.argmin_xor(query_mask) ^ query_mask) == transformed.front());
            assert((trie.argmax_xor(query_mask) ^ query_mask) == transformed.back());
            assert(trie.min_value() == optional<unsigned>(*values.begin()));
            assert(trie.max_value() == optional<unsigned>(*values.rbegin()));
        } else {
            assert(!trie.min_value() && !trie.max_value());
        }

        unsigned lower = rng() % 257, upper = rng() % 257;
        if (lower > upper) swap(lower, upper);
        int expected_less = count_if(transformed.begin(), transformed.end(),
                                     [&](unsigned x) { return x < upper; });
        int expected_less_equal = count_if(transformed.begin(), transformed.end(),
                                           [&](unsigned x) { return x <= min(upper, 256U); });
        int expected_freq = count_if(transformed.begin(), transformed.end(),
                                     [&](unsigned x) { return lower <= x && x < upper; });
        assert(trie.count_less(upper, query_mask) == expected_less);
        assert(trie.range_freq(lower, upper, query_mask) == expected_freq);
        if (upper < 256)
            assert(trie.count_less_equal(upper, query_mask) == expected_less_equal);

        auto expected_prev_it = lower_bound(transformed.begin(), transformed.end(), upper);
        optional<unsigned> expected_prev;
        if (expected_prev_it != transformed.begin()) expected_prev = *prev(expected_prev_it);
        auto expected_next_it = lower_bound(transformed.begin(), transformed.end(), lower);
        optional<unsigned> expected_next;
        if (expected_next_it != transformed.end()) expected_next = *expected_next_it;
        assert(trie.prev_value(upper, query_mask) == expected_prev);
        assert(trie.next_value(lower, query_mask) == expected_next);

        array<bool, 256> exists{};
        for (unsigned x : transformed) exists[x] = true;
        optional<unsigned> expected_mex;
        for (unsigned x = 0; x < 256; ++x) if (!exists[x]) {
            expected_mex = x;
            break;
        }
        assert(trie.mex(query_mask) == expected_mex);

        unsigned bound = rng() & 255U;
        auto plain_lower = values.lower_bound(bound);
        optional<unsigned> expected_lower, expected_upper;
        if (plain_lower != values.end()) expected_lower = *plain_lower;
        auto plain_upper = values.upper_bound(bound);
        if (plain_upper != values.end()) expected_upper = *plain_upper;
        assert(trie.lower_bound(bound) == expected_lower);
        assert(trie.upper_bound(bound) == expected_upper);
    }

    BinaryTrie<unsigned, 3> full;
    for (unsigned x = 0; x < 8; ++x) full.insert(x, 2);
    assert(!full.mex());
    full.erase(5);
    assert(!full.mex());
    full.erase(5);
    assert(full.mex() == optional<unsigned>(5));

    BinaryTrie<unsigned long long, 64> wide;
    wide.insert(0);
    assert(wide.mex() == optional<unsigned long long>(1));
    wide.xor_all(numeric_limits<unsigned long long>::max());
    assert(wide.count(numeric_limits<unsigned long long>::max()) == 1);

    trie.clear();
    assert(trie.empty() && trie.distinct_size() == 0 && trie.allocated_nodes() == 1);
}

static void transform_map(map<unsigned, long long>& values, unsigned mask) {
    map<unsigned, long long> transformed;
    for (auto [key, value] : values) transformed[key ^ mask] = value;
    values.swap(transformed);
}

static void test_monoid_trie() {
    mt19937 rng(42424242);
    BinaryTrieMonoid<unsigned, long long, bt_sum, bt_zero, 8> sum_trie;
    BinaryTrieMonoid<unsigned, long long, bt_max, bt_lowest, 8> max_trie;
    BinaryTrieSum<unsigned, long long, 8> sum_alias;
    BinaryTrieMax<unsigned, long long, 8> max_alias;
    map<unsigned, long long> values;

    for (int step = 0; step < 10000; ++step) {
        int type = rng() % 6;
        unsigned key = rng() & 255U;
        if (type <= 2) {
            long long payload = rng() % 30;
            values[key] = payload;
            sum_trie.set(key, payload);
            max_trie.set(key, payload);
            sum_alias.set(key, payload);
            max_alias.set(key, payload);
            assert(sum_trie.at(key) == payload);
            assert(sum_trie.get(key) == optional<long long>(payload));
        } else if (type == 3) {
            bool expected = values.erase(key);
            assert(sum_trie.erase(key) == expected);
            assert(max_trie.erase(key) == expected);
            assert(sum_alias.erase(key) == expected);
            assert(max_alias.erase(key) == expected);
        } else if (type == 4) {
            unsigned mask = rng() & 255U;
            transform_map(values, mask);
            sum_trie.xor_all(mask);
            max_trie.xor_all(mask);
            sum_alias.xor_all(mask);
            max_alias.xor_all(mask);
        }

        assert(sum_trie.size() == (int)values.size());
        assert(sum_trie.empty() == values.empty());
        assert(sum_trie.contains(key) == values.count(key));
        assert(sum_trie.get(key).has_value() == bool(values.count(key)));
        long long all_sum = 0, all_max = bt_lowest();
        for (auto [stored_key, payload] : values) {
            (void)stored_key;
            all_sum += payload;
            all_max = max(all_max, payload);
        }
        assert(sum_trie.all_prod() == all_sum && sum_alias.all_prod() == all_sum);
        assert(max_trie.all_prod() == all_max && max_alias.all_prod() == all_max);

        unsigned xor_mask = rng() & 255U;
        unsigned lower = rng() % 257, upper = rng() % 257;
        if (lower > upper) swap(lower, upper);
        long long expected_sum = 0, expected_max = bt_lowest();
        vector<pair<unsigned, long long>> ordered;
        for (auto [stored_key, payload] : values) {
            unsigned transformed = stored_key ^ xor_mask;
            ordered.push_back({transformed, payload});
            if (lower <= transformed && transformed < upper) {
                expected_sum += payload;
                expected_max = max(expected_max, payload);
            }
        }
        sort(ordered.begin(), ordered.end());
        assert(sum_trie.prod(lower, upper, xor_mask) == expected_sum);
        assert(sum_alias.prod(lower, upper, xor_mask) == expected_sum);
        assert(max_trie.prod(lower, upper, xor_mask) == expected_max);
        assert(max_alias.prod(lower, upper, xor_mask) == expected_max);

        long long expected_less = 0, expected_from = 0;
        int expected_count_less = 0;
        for (auto [transformed, payload] : ordered) {
            if (transformed < upper) expected_less += payload, ++expected_count_less;
            if (lower <= transformed) expected_from += payload;
        }
        assert(sum_trie.prod_less(upper, xor_mask) == expected_less);
        assert(sum_trie.prod_from(lower, xor_mask) == expected_from);
        assert(sum_trie.count_less(upper, xor_mask) == expected_count_less);
        if (!ordered.empty()) {
            int k = rng() % ordered.size();
            assert(sum_trie.kth_smallest(k, xor_mask) == ordered[k].first);
        }

        long long budget = rng() % 300;
        auto result = sum_trie.max_prefix_fold(
            [&](long long aggregate) { return aggregate <= budget; }, xor_mask);
        long long accepted_sum = 0;
        int accepted_keys = 0;
        optional<unsigned> first_rejected;
        for (auto [transformed, payload] : ordered) {
            if (accepted_sum + payload > budget) {
                first_rejected = transformed;
                break;
            }
            accepted_sum += payload;
            ++accepted_keys;
        }
        assert(result.first_rejected == first_rejected);
        assert(result.accepted_keys == accepted_keys);
        assert(result.fold == accepted_sum);
    }

    vector<unsigned> duplicate_keys = {3, 1, 3};
    vector<long long> payload = {4, 5, 9};
    BinaryTrieSum<unsigned, long long, 4> built(duplicate_keys, payload);
    assert(built.size() == 2 && built.at(3) == 9 && built.all_prod() == 14);
    built.clear();
    assert(built.empty() && built.all_prod() == 0 && built.allocated_nodes() == 1);
}

int main() {
    test_multiset_trie();
    test_monoid_trie();
    cout << "advanced binary trie tests passed\n";
}
