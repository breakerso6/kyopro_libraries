#pragma once
#include <bits/stdc++.h>

// A multiset of unsigned integers. Values are represented by their low BITS
// bits; insert/query values and XOR masks must fit in that range.
template<class UInt = unsigned, int BITS = std::numeric_limits<UInt>::digits>
struct BinaryTrie {
    static_assert(std::is_integral<UInt>::value && std::is_unsigned<UInt>::value,
                  "BinaryTrie requires an unsigned integral type");
    static_assert(1 <= BITS && BITS <= std::numeric_limits<UInt>::digits,
                  "invalid BITS");

    struct Node {
        int child[2] = {-1, -1};
        int count = 0;
        int distinct = 0;
    };

    std::vector<Node> nodes{Node{}};
    UInt lazy_xor = 0;

    BinaryTrie() = default;
    explicit BinaryTrie(const std::vector<UInt>& values) {
        reserve(values.size());
        for (UInt value : values) insert(value);
    }

    int size() const { return nodes[0].count; }
    int distinct_size() const { return nodes[0].distinct; }
    bool empty() const { return size() == 0; }
    int allocated_nodes() const { return (int)nodes.size(); }

    void reserve(size_t value_count) {
        nodes.reserve(1 + value_count * (size_t)BITS);
    }

    void clear() {
        nodes.assign(1, Node{});
        lazy_xor = 0;
    }

    int count(UInt value) const {
        assert(fits(value));
        return raw_count(value ^ lazy_xor);
    }

    bool contains(UInt value) const { return count(value) != 0; }

    void insert(UInt value) { insert(value, 1); }

    void insert(UInt value, int occurrences) {
        assert(fits(value) && occurrences > 0 && occurrences <= std::numeric_limits<int>::max() - size());
        UInt encoded = value ^ lazy_xor;
        std::array<int, BITS + 1> path{};
        int v = 0;
        path[0] = v;
        for (int bit = BITS - 1, depth = 1; bit >= 0; --bit, ++depth) {
            int direction = (encoded >> bit) & UInt(1);
            if (nodes[v].child[direction] == -1) {
                nodes[v].child[direction] = (int)nodes.size();
                nodes.push_back(Node{});
            }
            v = nodes[v].child[direction];
            path[depth] = v;
        }
        bool is_new = nodes[v].count == 0;
        for (int node : path) {
            nodes[node].count += occurrences;
            if (is_new) ++nodes[node].distinct;
        }
    }

    // Erases one occurrence if present.
    bool erase(UInt value) { return erase_up_to(value, 1) != 0; }

    // Erases up to occurrences copies and returns the number actually erased.
    int erase(UInt value, int occurrences) {
        assert(occurrences >= 0);
        return erase_up_to(value, occurrences);
    }

    int erase_all(UInt value) {
        assert(fits(value));
        return erase_up_to(value, count(value));
    }

    // Applies XOR to every logical value in O(1). Future insert/erase/query
    // operations use the transformed values.
    void xor_all(UInt mask) {
        assert(fits(mask));
        lazy_xor ^= mask;
    }

    // k-th smallest (stored_value xor xor_mask), 0-indexed.
    UInt kth_xor(UInt xor_mask, int k) const {
        assert(fits(xor_mask) && 0 <= k && k < size());
        UInt effective_mask = lazy_xor ^ xor_mask;
        UInt answer = 0;
        int v = 0;
        for (int bit = BITS - 1; bit >= 0; --bit) {
            int mask_bit = (effective_mask >> bit) & UInt(1);
            int zero_child = nodes[v].child[mask_bit];
            int zero_count = node_count(zero_child);
            int transformed_bit = 0;
            if (k >= zero_count) {
                k -= zero_count;
                transformed_bit = 1;
                answer |= UInt(1) << bit;
            }
            v = nodes[v].child[mask_bit ^ transformed_bit];
        }
        return answer;
    }

    UInt kth_smallest(int k) const { return kth_xor(0, k); }

    UInt kth_largest(int k) const {
        assert(0 <= k && k < size());
        return kth_xor(0, size() - 1 - k);
    }

    UInt min_xor(UInt value) const { return kth_xor(value, 0); }

    UInt max_xor(UInt value) const {
        assert(!empty());
        return kth_xor(value, size() - 1);
    }

    // Returns a stored logical value attaining the XOR optimum.
    UInt argmin_xor(UInt value) const { return value ^ min_xor(value); }
    UInt argmax_xor(UInt value) const { return value ^ max_xor(value); }

    std::optional<UInt> min_value() const {
        if (empty()) return std::nullopt;
        return kth_smallest(0);
    }

    std::optional<UInt> max_value() const {
        if (empty()) return std::nullopt;
        return kth_largest(0);
    }

    // Number of (stored_value xor xor_mask) values less than upper.
    int count_less(UInt upper, UInt xor_mask = 0) const {
        assert(fits(xor_mask));
        if (!fits(upper)) return size();
        UInt effective_mask = lazy_xor ^ xor_mask;
        int answer = 0, v = 0;
        for (int bit = BITS - 1; bit >= 0 && v != -1; --bit) {
            int mask_bit = (effective_mask >> bit) & UInt(1);
            if ((upper >> bit) & UInt(1)) {
                answer += node_count(nodes[v].child[mask_bit]);
                v = nodes[v].child[mask_bit ^ 1];
            } else {
                v = nodes[v].child[mask_bit];
            }
        }
        return answer;
    }

    int count_less_equal(UInt upper, UInt xor_mask = 0) const {
        assert(fits(xor_mask));
        if (!fits(upper)) return size();
        return count_less(upper, xor_mask) + count(upper ^ xor_mask);
    }

    int range_freq(UInt lower, UInt upper, UInt xor_mask = 0) const {
        assert(lower <= upper);
        return count_less(upper, xor_mask) - count_less(lower, xor_mask);
    }

    std::optional<UInt> prev_value(UInt upper, UInt xor_mask = 0) const {
        int rank = count_less(upper, xor_mask);
        if (rank == 0) return std::nullopt;
        return kth_xor(xor_mask, rank - 1);
    }

    std::optional<UInt> next_value(UInt lower, UInt xor_mask = 0) const {
        int rank = count_less(lower, xor_mask);
        if (rank == size()) return std::nullopt;
        return kth_xor(xor_mask, rank);
    }

    std::optional<UInt> lower_bound(UInt value) const { return next_value(value); }

    std::optional<UInt> upper_bound(UInt value) const {
        int rank = count_less_equal(value);
        if (rank == size()) return std::nullopt;
        return kth_smallest(rank);
    }

    // Minimum missing value among (stored_value xor xor_mask), ignoring
    // duplicates. nullopt means the complete BITS-bit universe is present.
    std::optional<UInt> mex(UInt xor_mask = 0) const {
        assert(fits(xor_mask));
        if (subtree_full(0, BITS)) return std::nullopt;
        UInt effective_mask = lazy_xor ^ xor_mask;
        UInt answer = 0;
        int v = 0;
        for (int bit = BITS - 1; bit >= 0; --bit) {
            int mask_bit = (effective_mask >> bit) & UInt(1);
            int zero_child = v == -1 ? -1 : nodes[v].child[mask_bit];
            if (!subtree_full(zero_child, bit)) {
                v = zero_child;
            } else {
                answer |= UInt(1) << bit;
                v = v == -1 ? -1 : nodes[v].child[mask_bit ^ 1];
            }
        }
        return answer;
    }

private:
    static bool fits(UInt value) {
        if constexpr (BITS == std::numeric_limits<UInt>::digits) return true;
        else return (value >> BITS) == 0;
    }

    int node_count(int v) const { return v == -1 ? 0 : nodes[v].count; }
    int node_distinct(int v) const { return v == -1 ? 0 : nodes[v].distinct; }

    int raw_count(UInt encoded) const {
        int v = 0;
        for (int bit = BITS - 1; bit >= 0; --bit) {
            v = nodes[v].child[(encoded >> bit) & UInt(1)];
            if (v == -1) return 0;
        }
        return nodes[v].count;
    }

    int erase_up_to(UInt value, int occurrences) {
        assert(fits(value));
        if (occurrences == 0) return 0;
        UInt encoded = value ^ lazy_xor;
        std::array<int, BITS + 1> path{};
        int v = 0;
        path[0] = v;
        for (int bit = BITS - 1, depth = 1; bit >= 0; --bit, ++depth) {
            v = nodes[v].child[(encoded >> bit) & UInt(1)];
            if (v == -1) return 0;
            path[depth] = v;
        }
        if (nodes[v].count == 0) return 0;
        int erased = std::min(occurrences, nodes[v].count);
        bool disappears = erased == nodes[v].count;
        for (int node : path) {
            nodes[node].count -= erased;
            if (disappears) --nodes[node].distinct;
        }
        return erased;
    }

    bool subtree_full(int v, int remaining_bits) const {
        if (v == -1) return false;
        // int cannot hold 2^31 distinct values, so such a subtree cannot be full.
        if (remaining_bits >= 31) return false;
        return node_distinct(v) == (1 << remaining_bits);
    }
};

namespace binary_trie_detail {

template<class S> S add(S a, S b) { return a + b; }
template<class S> S zero() { return S(0); }
template<class S> S maximum(S a, S b) { return std::max(a, b); }
template<class S> S lowest() { return std::numeric_limits<S>::lowest(); }

} // namespace binary_trie_detail

// A dynamic UInt -> S map with subtree aggregation. The operation must be a
// commutative monoid because query-time XOR can exchange child order.
template<class UInt, class S, S (*op)(S, S), S (*e)(),
         int BITS = std::numeric_limits<UInt>::digits>
struct BinaryTrieMonoid {
    static_assert(std::is_integral<UInt>::value && std::is_unsigned<UInt>::value,
                  "BinaryTrieMonoid requires an unsigned integral key type");
    static_assert(1 <= BITS && BITS <= std::numeric_limits<UInt>::digits,
                  "invalid BITS");
    static_assert(std::numeric_limits<UInt>::digits <= 64,
                  "BinaryTrieMonoid supports key types of at most 64 bits");

    struct PrefixFoldResult {
        std::optional<UInt> first_rejected;
        int accepted_keys;
        S fold;
    };

    struct Node {
        int child[2] = {-1, -1};
        int count = 0;
        S aggregate;
        Node() : aggregate(e()) {}
    };

    std::vector<Node> nodes{Node{}};
    UInt lazy_xor = 0;

    BinaryTrieMonoid() = default;
    BinaryTrieMonoid(const std::vector<UInt>& keys, const std::vector<S>& payload) {
        build(keys, payload);
    }

    void build(const std::vector<UInt>& keys, const std::vector<S>& payload) {
        assert(keys.size() == payload.size());
        clear();
        reserve(keys.size());
        for (int i = 0; i < (int)keys.size(); ++i) set(keys[i], payload[i]);
    }

    int size() const { return nodes[0].count; }
    bool empty() const { return size() == 0; }
    int allocated_nodes() const { return (int)nodes.size(); }

    void reserve(size_t key_count) {
        nodes.reserve(1 + key_count * (size_t)BITS);
    }

    void clear() {
        nodes.assign(1, Node{});
        lazy_xor = 0;
    }

    bool contains(UInt key) const { return find_node(key) != -1; }

    std::optional<S> get(UInt key) const {
        int v = find_node(key);
        if (v == -1) return std::nullopt;
        return nodes[v].aggregate;
    }

    S at(UInt key) const {
        auto result = get(key);
        assert(result.has_value());
        return *result;
    }

    // Inserts or assigns one payload at key. Duplicate keys are overwritten.
    void set(UInt key, const S& value) {
        assert(fits(key));
        UInt encoded = key ^ lazy_xor;
        std::array<int, BITS + 1> path{};
        int v = 0;
        path[0] = v;
        for (int bit = BITS - 1, depth = 1; bit >= 0; --bit, ++depth) {
            int direction = (encoded >> bit) & UInt(1);
            if (nodes[v].child[direction] == -1) {
                nodes[v].child[direction] = (int)nodes.size();
                nodes.push_back(Node{});
            }
            v = nodes[v].child[direction];
            path[depth] = v;
        }
        nodes[v].count = 1;
        nodes[v].aggregate = value;
        rebuild_path(path);
    }

    bool erase(UInt key) {
        assert(fits(key));
        UInt encoded = key ^ lazy_xor;
        std::array<int, BITS + 1> path{};
        int v = 0;
        path[0] = v;
        for (int bit = BITS - 1, depth = 1; bit >= 0; --bit, ++depth) {
            v = nodes[v].child[(encoded >> bit) & UInt(1)];
            if (v == -1) return false;
            path[depth] = v;
        }
        if (nodes[v].count == 0) return false;
        nodes[v].count = 0;
        nodes[v].aggregate = e();
        rebuild_path(path);
        return true;
    }

    void xor_all(UInt mask) {
        assert(fits(mask));
        lazy_xor ^= mask;
    }

    S all_prod() const { return nodes[0].aggregate; }

    // Aggregate for lower <= (key xor xor_mask) < upper.
    S prod(UInt lower, UInt upper, UInt xor_mask = 0) const {
        assert(lower <= upper && fits(xor_mask));
        using Wide = __uint128_t;
        Wide universe = Wide(1) << BITS;
        Wide ql = std::min(Wide(lower), universe);
        Wide qr = std::min(Wide(upper), universe);
        if (ql >= qr) return e();
        return prod_dfs(0, 0, 0, universe, ql, qr, lazy_xor ^ xor_mask);
    }

    S prod_less(UInt upper, UInt xor_mask = 0) const {
        assert(fits(xor_mask));
        using Wide = __uint128_t;
        Wide universe = Wide(1) << BITS;
        Wide qr = std::min(Wide(upper), universe);
        return prod_dfs(0, 0, 0, universe, 0, qr, lazy_xor ^ xor_mask);
    }

    S prod_from(UInt lower, UInt xor_mask = 0) const {
        assert(fits(xor_mask));
        using Wide = __uint128_t;
        Wide universe = Wide(1) << BITS;
        Wide ql = std::min(Wide(lower), universe);
        return prod_dfs(0, 0, 0, universe, ql, universe, lazy_xor ^ xor_mask);
    }

    int count_less(UInt upper, UInt xor_mask = 0) const {
        assert(fits(xor_mask));
        if (!fits(upper)) return size();
        UInt effective_mask = lazy_xor ^ xor_mask;
        int answer = 0, v = 0;
        for (int bit = BITS - 1; bit >= 0 && v != -1; --bit) {
            int mask_bit = (effective_mask >> bit) & UInt(1);
            if ((upper >> bit) & UInt(1)) {
                answer += node_count(nodes[v].child[mask_bit]);
                v = nodes[v].child[mask_bit ^ 1];
            } else {
                v = nodes[v].child[mask_bit];
            }
        }
        return answer;
    }

    UInt kth_smallest(int k, UInt xor_mask = 0) const {
        assert(fits(xor_mask) && 0 <= k && k < size());
        UInt effective_mask = lazy_xor ^ xor_mask;
        UInt answer = 0;
        int v = 0;
        for (int bit = BITS - 1; bit >= 0; --bit) {
            int mask_bit = (effective_mask >> bit) & UInt(1);
            int left = nodes[v].child[mask_bit];
            int left_count = node_count(left);
            int transformed_bit = 0;
            if (k >= left_count) {
                k -= left_count;
                transformed_bit = 1;
                answer |= UInt(1) << bit;
            }
            v = nodes[v].child[mask_bit ^ transformed_bit];
        }
        return answer;
    }

    // Finds the first key (in key xor xor_mask order) that would make pred
    // false when its whole payload is appended. pred(e()) must be true and
    // monotone under prefix extension.
    template<class Predicate>
    PrefixFoldResult max_prefix_fold(Predicate pred, UInt xor_mask = 0) const {
        assert(fits(xor_mask) && pred(e()));
        if (pred(all_prod())) return {std::nullopt, size(), all_prod()};
        UInt effective_mask = lazy_xor ^ xor_mask;
        UInt first_rejected = 0;
        S accumulated = e();
        int accepted = 0, v = 0;
        for (int bit = BITS - 1; bit >= 0; --bit) {
            int mask_bit = (effective_mask >> bit) & UInt(1);
            int left = nodes[v].child[mask_bit];
            S with_left = op(accumulated, node_aggregate(left));
            if (pred(with_left)) {
                accumulated = with_left;
                accepted += node_count(left);
                first_rejected |= UInt(1) << bit;
                v = nodes[v].child[mask_bit ^ 1];
            } else {
                v = left;
            }
            assert(v != -1);
        }
        return {first_rejected, accepted, accumulated};
    }

private:
    static bool fits(UInt value) {
        if constexpr (BITS == std::numeric_limits<UInt>::digits) return true;
        else return (value >> BITS) == 0;
    }

    int node_count(int v) const { return v == -1 ? 0 : nodes[v].count; }
    S node_aggregate(int v) const { return v == -1 ? e() : nodes[v].aggregate; }

    int find_node(UInt key) const {
        assert(fits(key));
        UInt encoded = key ^ lazy_xor;
        int v = 0;
        for (int bit = BITS - 1; bit >= 0; --bit) {
            v = nodes[v].child[(encoded >> bit) & UInt(1)];
            if (v == -1) return -1;
        }
        return nodes[v].count ? v : -1;
    }

    void rebuild_path(const std::array<int, BITS + 1>& path) {
        for (int depth = BITS - 1; depth >= 0; --depth) {
            int v = path[depth];
            nodes[v].count = node_count(nodes[v].child[0]) + node_count(nodes[v].child[1]);
            nodes[v].aggregate = op(node_aggregate(nodes[v].child[0]),
                                    node_aggregate(nodes[v].child[1]));
        }
    }

    S prod_dfs(int v, int depth, __uint128_t value_l, __uint128_t value_r,
               __uint128_t query_l, __uint128_t query_r, UInt effective_mask) const {
        if (v == -1 || nodes[v].count == 0 || value_r <= query_l || query_r <= value_l) return e();
        if (query_l <= value_l && value_r <= query_r) return nodes[v].aggregate;
        if (depth == BITS) return nodes[v].aggregate;
        int bit = BITS - 1 - depth;
        int mask_bit = (effective_mask >> bit) & UInt(1);
        __uint128_t mid = (value_l + value_r) / 2;
        return op(prod_dfs(nodes[v].child[mask_bit], depth + 1, value_l, mid,
                           query_l, query_r, effective_mask),
                  prod_dfs(nodes[v].child[mask_bit ^ 1], depth + 1, mid, value_r,
                           query_l, query_r, effective_mask));
    }
};

template<class UInt, class S, int BITS = std::numeric_limits<UInt>::digits>
using BinaryTrieSum = BinaryTrieMonoid<
    UInt, S, binary_trie_detail::add<S>, binary_trie_detail::zero<S>, BITS>;

template<class UInt, class S, int BITS = std::numeric_limits<UInt>::digits>
using BinaryTrieMax = BinaryTrieMonoid<
    UInt, S, binary_trie_detail::maximum<S>, binary_trie_detail::lowest<S>, BITS>;
