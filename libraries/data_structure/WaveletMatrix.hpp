#pragma once
#include <bits/stdc++.h>

// A coordinate-compressed wavelet matrix. All index and value ranges are
// half-open. T only needs to be copyable and strictly ordered by operator<.
template<class T>
struct WaveletMatrix {
    struct RangeSegment {
        int depth, left, right;
    };

    int n = 0, levels = 0;
    std::vector<T> values;
    std::vector<int> original_code;
    std::vector<std::vector<int>> prefix_one;
    std::vector<int> zero_count;

    WaveletMatrix() = default;
    explicit WaveletMatrix(const std::vector<T>& a) { build(a); }

    void build(const std::vector<T>& a) {
        n = (int)a.size();
        values = a;
        std::sort(values.begin(), values.end());
        values.erase(std::unique(values.begin(), values.end(), [](const T& x, const T& y) {
            return !(x < y) && !(y < x);
        }), values.end());
        levels = 0;
        while ((1ULL << levels) < std::max<size_t>(1, values.size())) ++levels;
        levels = std::max(levels, 1);

        prefix_one.assign(levels, std::vector<int>(n + 1));
        zero_count.assign(levels, 0);
        original_code.resize(n);
        for (int i = 0; i < n; ++i)
            original_code[i] = (int)(std::lower_bound(values.begin(), values.end(), a[i]) - values.begin());

        std::vector<int> cur = original_code, next(n);
        for (int depth = 0; depth < levels; ++depth) {
            int bit = levels - 1 - depth;
            for (int i = 0; i < n; ++i)
                prefix_one[depth][i + 1] = prefix_one[depth][i] + ((cur[i] >> bit) & 1);
            zero_count[depth] = n - prefix_one[depth][n];
            int zi = 0, oi = zero_count[depth];
            for (int code : cur) next[((code >> bit) & 1) ? oi++ : zi++] = code;
            cur.swap(next);
        }
    }

    int size() const { return n; }
    int alphabet_size() const { return (int)values.size(); }
    bool empty() const { return n == 0; }

    T access(int index) const {
        assert(0 <= index && index < n);
        int code = 0;
        for (int depth = 0; depth < levels; ++depth) {
            int ones = prefix_one[depth][index];
            int bit = prefix_one[depth][index + 1] - ones;
            code = (code << 1) | bit;
            index = bit ? zero_count[depth] + ones : index - ones;
        }
        return values[code];
    }
    T operator[](int index) const { return access(index); }

    T kth_smallest(int l, int r, int k) const {
        assert(0 <= l && l <= r && r <= n && 0 <= k && k < r - l);
        int code = 0;
        for (int depth = 0; depth < levels; ++depth) {
            int ones_l = prefix_one[depth][l], ones_r = prefix_one[depth][r];
            int zeros = (r - l) - (ones_r - ones_l);
            code <<= 1;
            if (k < zeros) {
                l -= ones_l;
                r -= ones_r;
            } else {
                code |= 1;
                k -= zeros;
                l = zero_count[depth] + ones_l;
                r = zero_count[depth] + ones_r;
            }
        }
        return values[code];
    }

    T kth_largest(int l, int r, int k) const {
        assert(0 <= l && l <= r && r <= n && 0 <= k && k < r - l);
        return kth_smallest(l, r, r - l - 1 - k);
    }

    int count_less(int l, int r, const T& upper) const {
        assert(0 <= l && l <= r && r <= n);
        int code = (int)(std::lower_bound(values.begin(), values.end(), upper) - values.begin());
        return count_code_less(l, r, code);
    }

    int count_less_equal(int l, int r, const T& upper) const {
        assert(0 <= l && l <= r && r <= n);
        int code = (int)(std::upper_bound(values.begin(), values.end(), upper) - values.begin());
        return count_code_less(l, r, code);
    }

    int range_freq(int l, int r, const T& lower, const T& upper) const {
        assert(!(upper < lower));
        return count_less(l, r, upper) - count_less(l, r, lower);
    }

    // Number of elements in [l, r) whose value is in [lower, +infinity).
    int range_freq(int l, int r, const T& lower) const {
        assert(0 <= l && l <= r && r <= n);
        return r - l - count_less(l, r, lower);
    }

    int count(int l, int r, const T& value) const {
        assert(0 <= l && l <= r && r <= n);
        auto it = std::lower_bound(values.begin(), values.end(), value);
        if (it == values.end() || value < *it || *it < value) return 0;
        int code = (int)(it - values.begin());
        for (int depth = 0; depth < levels; ++depth) {
            int ones_l = prefix_one[depth][l], ones_r = prefix_one[depth][r];
            int bit = levels - 1 - depth;
            if ((code >> bit) & 1) {
                l = zero_count[depth] + ones_l;
                r = zero_count[depth] + ones_r;
            } else {
                l -= ones_l;
                r -= ones_r;
            }
        }
        return r - l;
    }

    int rank(const T& value, int r) const {
        assert(0 <= r && r <= n);
        return count(0, r, value);
    }
    int rank(const T& value, int l, int r) const { return count(l, r, value); }

    // Returns the index of the kth (0-indexed) occurrence, or nullopt.
    // This convenience operation is O(log(sigma) log(n)).
    std::optional<int> select(const T& value, int k) const {
        if (k < 0 || count(0, n, value) <= k) return std::nullopt;
        int low = 0, high = n;
        while (low < high) {
            int mid = low + (high - low) / 2;
            if (rank(value, mid + 1) >= k + 1) high = mid;
            else low = mid + 1;
        }
        return low;
    }

    std::optional<T> prev_value(int l, int r, const T& upper) const {
        int cnt = count_less(l, r, upper);
        if (cnt == 0) return std::nullopt;
        return kth_smallest(l, r, cnt - 1);
    }

    std::optional<T> next_value(int l, int r, const T& lower) const {
        int cnt = count_less(l, r, lower);
        if (cnt == r - l) return std::nullopt;
        return kth_smallest(l, r, cnt);
    }

    std::optional<T> range_min(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return std::nullopt;
        return kth_smallest(l, r, 0);
    }

    std::optional<T> range_max(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return std::nullopt;
        return kth_largest(l, r, 0);
    }

    // Decomposes [l, r) x [lower, upper) into O(log(sigma)) wavelet-matrix
    // nodes. A segment at depth d refers to [left, right) in the sequence
    // after d stable partitions. This is the extension point for attaching
    // arbitrary static/dynamic range-query data structures to every depth.
    std::vector<RangeSegment> range_segments(
        int l, int r, const T& lower, const T& upper) const {
        assert(0 <= l && l <= r && r <= n && !(upper < lower));
        int ql = (int)(std::lower_bound(values.begin(), values.end(), lower) - values.begin());
        int qr = (int)(std::lower_bound(values.begin(), values.end(), upper) - values.begin());
        return range_segments_code(l, r, ql, qr);
    }

    std::vector<RangeSegment> range_segments_less(int l, int r, const T& upper) const {
        assert(0 <= l && l <= r && r <= n);
        int qr = (int)(std::lower_bound(values.begin(), values.end(), upper) - values.begin());
        return range_segments_code(l, r, 0, qr);
    }

    // payload[d] is payload reordered in exactly the sequence used at depth d.
    template<class S>
    std::vector<std::vector<S>> reorder_each_level(const std::vector<S>& payload) const {
        assert((int)payload.size() == n);
        std::vector<std::vector<S>> result(levels + 1, std::vector<S>(n));
        result[0] = payload;
        std::vector<int> cur_code = original_code, next_code(n);
        for (int depth = 0; depth < levels; ++depth) {
            int bit = levels - 1 - depth;
            int zi = 0, oi = zero_count[depth];
            for (int i = 0; i < n; ++i) {
                int& to = ((cur_code[i] >> bit) & 1) ? oi : zi;
                next_code[to] = cur_code[i];
                result[depth + 1][to] = result[depth][i];
                ++to;
            }
            cur_code.swap(next_code);
        }
        return result;
    }

    std::array<std::pair<int, int>, 2> children(int depth, int l, int r) const {
        assert(0 <= depth && depth < levels && 0 <= l && l <= r && r <= n);
        int ones_l = prefix_one[depth][l], ones_r = prefix_one[depth][r];
        return {{{l - ones_l, r - ones_r},
                 {zero_count[depth] + ones_l, zero_count[depth] + ones_r}}};
    }

private:
    int count_code_less(int l, int r, int code) const {
        if (code <= 0) return 0;
        if (code >= (int)values.size()) return r - l;
        int answer = 0;
        for (int depth = 0; depth < levels; ++depth) {
            int bit = levels - 1 - depth;
            int ones_l = prefix_one[depth][l], ones_r = prefix_one[depth][r];
            if ((code >> bit) & 1) {
                answer += (r - l) - (ones_r - ones_l);
                l = zero_count[depth] + ones_l;
                r = zero_count[depth] + ones_r;
            } else {
                l -= ones_l;
                r -= ones_r;
            }
        }
        return answer;
    }

    std::vector<RangeSegment> range_segments_code(int l, int r, int ql, int qr) const {
        std::vector<RangeSegment> result;
        if (l == r || ql >= qr) return result;
        uint64_t universe = 1ULL << levels;
        auto dfs = [&](auto&& self, int depth, int left, int right,
                       uint64_t value_l, uint64_t value_r) -> void {
            if (left == right || value_r <= (uint64_t)ql || (uint64_t)qr <= value_l) return;
            if ((uint64_t)ql <= value_l && value_r <= (uint64_t)qr) {
                result.push_back({depth, left, right});
                return;
            }
            if (depth == levels) return;
            auto child = children(depth, left, right);
            uint64_t mid = (value_l + value_r) / 2;
            self(self, depth + 1, child[0].first, child[0].second, value_l, mid);
            self(self, depth + 1, child[1].first, child[1].second, mid, value_r);
        };
        dfs(dfs, 0, l, r, 0, universe);
        return result;
    }
};

// Integer-only wavelet matrix whose order can be changed by a query-time XOR.
// LOG is the number of significant low bits and defaults to all value bits.
template<class UInt, int LOG = std::numeric_limits<UInt>::digits>
struct XorWaveletMatrix {
    static_assert(std::is_integral<UInt>::value && std::is_unsigned<UInt>::value,
                  "XorWaveletMatrix requires an unsigned integral type");
    static_assert(1 <= LOG && LOG <= std::numeric_limits<UInt>::digits, "invalid LOG");

    int n = 0;
    std::vector<std::vector<int>> prefix_one;
    std::vector<int> zero_count;

    XorWaveletMatrix() = default;
    explicit XorWaveletMatrix(const std::vector<UInt>& a) { build(a); }

    void build(const std::vector<UInt>& a) {
        n = (int)a.size();
        for (UInt value : a) assert(fits(value));
        prefix_one.assign(LOG, std::vector<int>(n + 1));
        zero_count.assign(LOG, 0);
        std::vector<UInt> cur = a, next(n);
        for (int depth = 0; depth < LOG; ++depth) {
            int bit = LOG - 1 - depth;
            for (int i = 0; i < n; ++i)
                prefix_one[depth][i + 1] = prefix_one[depth][i] + ((cur[i] >> bit) & UInt(1));
            zero_count[depth] = n - prefix_one[depth][n];
            int zi = 0, oi = zero_count[depth];
            for (UInt value : cur) next[((value >> bit) & UInt(1)) ? oi++ : zi++] = value;
            cur.swap(next);
        }
    }

    int size() const { return n; }
    bool empty() const { return n == 0; }

    UInt access(int index) const {
        assert(0 <= index && index < n);
        UInt value = 0;
        for (int depth = 0; depth < LOG; ++depth) {
            int ones = prefix_one[depth][index];
            int bit = prefix_one[depth][index + 1] - ones;
            value |= UInt(bit) << (LOG - 1 - depth);
            index = bit ? zero_count[depth] + ones : index - ones;
        }
        return value;
    }

    UInt kth_smallest(int l, int r, int k, UInt xor_mask = 0) const {
        assert(0 <= l && l <= r && r <= n && 0 <= k && k < r - l && fits(xor_mask));
        UInt answer = 0;
        for (int depth = 0; depth < LOG; ++depth) {
            int bit = LOG - 1 - depth;
            int ones_l = prefix_one[depth][l], ones_r = prefix_one[depth][r];
            int original_zero = (r - l) - (ones_r - ones_l);
            bool mask_bit = (xor_mask >> bit) & UInt(1);
            int transformed_zero = mask_bit ? (ones_r - ones_l) : original_zero;
            bool transformed_bit = k >= transformed_zero;
            if (transformed_bit) {
                answer |= UInt(1) << bit;
                k -= transformed_zero;
            }
            bool original_bit = transformed_bit ^ mask_bit;
            if (original_bit) {
                l = zero_count[depth] + ones_l;
                r = zero_count[depth] + ones_r;
            } else {
                l -= ones_l;
                r -= ones_r;
            }
        }
        return answer;
    }

    UInt kth_largest(int l, int r, int k, UInt xor_mask = 0) const {
        assert(0 <= l && l <= r && r <= n && 0 <= k && k < r - l);
        return kth_smallest(l, r, r - l - 1 - k, xor_mask);
    }

    UInt min_xor(int l, int r, UInt x) const { return kth_smallest(l, r, 0, x); }
    UInt max_xor(int l, int r, UInt x) const { return kth_largest(l, r, 0, x); }

    int count_less(int l, int r, UInt upper, UInt xor_mask = 0) const {
        assert(0 <= l && l <= r && r <= n && fits(xor_mask));
        if (!fits(upper)) return r - l;
        int answer = 0;
        for (int depth = 0; depth < LOG; ++depth) {
            int bit = LOG - 1 - depth;
            int ones_l = prefix_one[depth][l], ones_r = prefix_one[depth][r];
            int original_zero = (r - l) - (ones_r - ones_l);
            bool mask_bit = (xor_mask >> bit) & UInt(1);
            int transformed_zero = mask_bit ? (ones_r - ones_l) : original_zero;
            bool upper_bit = (upper >> bit) & UInt(1);
            bool transformed_bit = false;
            if (upper_bit) {
                answer += transformed_zero;
                transformed_bit = true;
            }
            bool original_bit = transformed_bit ^ mask_bit;
            if (original_bit) {
                l = zero_count[depth] + ones_l;
                r = zero_count[depth] + ones_r;
            } else {
                l -= ones_l;
                r -= ones_r;
            }
        }
        return answer;
    }

    int range_freq(int l, int r, UInt lower, UInt upper, UInt xor_mask = 0) const {
        assert(lower <= upper);
        return count_less(l, r, upper, xor_mask) - count_less(l, r, lower, xor_mask);
    }

    int count(int l, int r, UInt value, UInt xor_mask = 0) const {
        assert(0 <= l && l <= r && r <= n && fits(value) && fits(xor_mask));
        UInt original = value ^ xor_mask;
        for (int depth = 0; depth < LOG; ++depth) {
            int bit = LOG - 1 - depth;
            int ones_l = prefix_one[depth][l], ones_r = prefix_one[depth][r];
            if ((original >> bit) & UInt(1)) {
                l = zero_count[depth] + ones_l;
                r = zero_count[depth] + ones_r;
            } else {
                l -= ones_l;
                r -= ones_r;
            }
        }
        return r - l;
    }

private:
    static bool fits(UInt value) {
        if constexpr (LOG == std::numeric_limits<UInt>::digits) return true;
        else return (value >> LOG) == 0;
    }
};

namespace wavelet_matrix_detail {

template<class S, S (*op)(S, S), S (*e)()>
struct SegmentTree {
    int n = 0, size = 1;
    std::vector<S> data;

    SegmentTree() = default;
    explicit SegmentTree(const std::vector<S>& a) { build(a); }

    void build(const std::vector<S>& a) {
        n = (int)a.size();
        size = 1;
        while (size < std::max(1, n)) size <<= 1;
        data.assign(2 * size, e());
        for (int i = 0; i < n; ++i) data[size + i] = a[i];
        for (int i = size - 1; i; --i) data[i] = op(data[i << 1], data[i << 1 | 1]);
    }

    void set(int p, const S& value) {
        assert(0 <= p && p < n);
        data[p += size] = value;
        while (p >>= 1) data[p] = op(data[p << 1], data[p << 1 | 1]);
    }

    S prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        S left = e(), right = e();
        for (l += size, r += size; l < r; l >>= 1, r >>= 1) {
            if (l & 1) left = op(left, data[l++]);
            if (r & 1) right = op(data[--r], right);
        }
        return op(left, right);
    }
};

template<class S> S add(S a, S b) { return a + b; }
template<class S> S zero() { return S(0); }
template<class S> S negate(S a) { return -a; }
template<class S> S maximum(S a, S b) { return std::max(a, b); }
template<class S> S lowest() { return std::numeric_limits<S>::lowest(); }

} // namespace wavelet_matrix_detail

// Static payload aggregation over an abelian group. Prefix sums at every
// depth give O(log(sigma)) queries and O(n log(sigma)) memory.
template<class Key, class S, S (*op)(S, S), S (*e)(), S (*inverse)(S)>
struct WaveletMatrixGroup {
    struct PrefixFoldResult {
        int distinct_count;
        S fold;
    };

    WaveletMatrix<Key> matrix;
    std::vector<std::vector<S>> prefix;

    WaveletMatrixGroup() = default;
    WaveletMatrixGroup(const std::vector<Key>& keys, const std::vector<S>& weights) {
        build(keys, weights);
    }

    void build(const std::vector<Key>& keys, const std::vector<S>& weights) {
        assert(keys.size() == weights.size());
        matrix.build(keys);
        auto reordered = matrix.reorder_each_level(weights);
        prefix.assign(matrix.levels + 1, std::vector<S>(matrix.n + 1, e()));
        for (int depth = 0; depth <= matrix.levels; ++depth)
            for (int i = 0; i < matrix.n; ++i)
                prefix[depth][i + 1] = op(prefix[depth][i], reordered[depth][i]);
    }

    int size() const { return matrix.size(); }

    S prod(int l, int r, const Key& lower, const Key& upper) const {
        S answer = e();
        for (auto segment : matrix.range_segments(l, r, lower, upper))
            answer = op(answer, node_prod(segment.depth, segment.left, segment.right));
        return answer;
    }

    S prod_less(int l, int r, const Key& upper) const {
        S answer = e();
        for (auto segment : matrix.range_segments_less(l, r, upper))
            answer = op(answer, node_prod(segment.depth, segment.left, segment.right));
        return answer;
    }

    S all_prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        return node_prod(0, l, r);
    }

    // Appends whole equal-key buckets in ascending key order while pred holds.
    // pred(e()) must be true and pred must be monotone. distinct_count tells
    // how many global distinct keys were accepted; fold is their aggregate.
    template<class Predicate>
    PrefixFoldResult max_prefix_fold(int l, int r, Predicate pred) const {
        assert(0 <= l && l <= r && r <= size() && pred(e()));
        S accumulated = e();
        int code = 0;
        for (int depth = 0; depth < matrix.levels; ++depth) {
            auto child = matrix.children(depth, l, r);
            S with_left = op(accumulated, node_prod(depth + 1, child[0].first, child[0].second));
            code <<= 1;
            if (pred(with_left)) {
                accumulated = with_left;
                code |= 1;
                l = child[1].first;
                r = child[1].second;
            } else {
                l = child[0].first;
                r = child[0].second;
            }
        }
        int sigma = matrix.alphabet_size();
        if (code >= sigma) return {sigma, accumulated};
        S with_leaf = op(accumulated, node_prod(matrix.levels, l, r));
        if (pred(with_leaf)) return {std::min(code + 1, sigma), with_leaf};
        return {code, accumulated};
    }

private:
    S node_prod(int depth, int l, int r) const {
        return op(inverse(prefix[depth][l]), prefix[depth][r]);
    }
};

// Payload aggregation over a commutative monoid with point assignment.
// Queries and updates take O(log(sigma) log(n)); keys remain static.
template<class Key, class S, S (*op)(S, S), S (*e)()>
struct WaveletMatrixMonoid {
    struct PrefixFoldResult {
        int distinct_count;
        S fold;
    };

    WaveletMatrix<Key> matrix;
    std::vector<wavelet_matrix_detail::SegmentTree<S, op, e>> segment_tree;
    std::vector<std::vector<int>> position;
    std::vector<S> weights;

    WaveletMatrixMonoid() = default;
    WaveletMatrixMonoid(const std::vector<Key>& keys, const std::vector<S>& initial_weights) {
        build(keys, initial_weights);
    }

    void build(const std::vector<Key>& keys, const std::vector<S>& initial_weights) {
        assert(keys.size() == initial_weights.size());
        matrix.build(keys);
        weights = initial_weights;
        auto reordered = matrix.reorder_each_level(initial_weights);
        segment_tree.resize(matrix.levels + 1);
        for (int depth = 0; depth <= matrix.levels; ++depth)
            segment_tree[depth].build(reordered[depth]);

        position.assign(matrix.levels + 1, std::vector<int>(matrix.n));
        for (int original = 0; original < matrix.n; ++original) {
            int p = original;
            position[0][original] = p;
            int code = matrix.original_code[original];
            for (int depth = 0; depth < matrix.levels; ++depth) {
                int ones = matrix.prefix_one[depth][p];
                int bit = matrix.levels - 1 - depth;
                p = ((code >> bit) & 1) ? matrix.zero_count[depth] + ones : p - ones;
                position[depth + 1][original] = p;
            }
        }
    }

    int size() const { return matrix.size(); }

    void set(int index, const S& value) {
        assert(0 <= index && index < size());
        weights[index] = value;
        for (int depth = 0; depth <= matrix.levels; ++depth)
            segment_tree[depth].set(position[depth][index], value);
    }

    const S& get(int index) const {
        assert(0 <= index && index < size());
        return weights[index];
    }

    S prod(int l, int r, const Key& lower, const Key& upper) const {
        S answer = e();
        for (auto segment : matrix.range_segments(l, r, lower, upper))
            answer = op(answer, node_prod(segment.depth, segment.left, segment.right));
        return answer;
    }

    S prod_less(int l, int r, const Key& upper) const {
        S answer = e();
        for (auto segment : matrix.range_segments_less(l, r, upper))
            answer = op(answer, node_prod(segment.depth, segment.left, segment.right));
        return answer;
    }

    S all_prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        return node_prod(0, l, r);
    }

    template<class Predicate>
    PrefixFoldResult max_prefix_fold(int l, int r, Predicate pred) const {
        assert(0 <= l && l <= r && r <= size() && pred(e()));
        S accumulated = e();
        int code = 0;
        for (int depth = 0; depth < matrix.levels; ++depth) {
            auto child = matrix.children(depth, l, r);
            S with_left = op(accumulated, node_prod(depth + 1, child[0].first, child[0].second));
            code <<= 1;
            if (pred(with_left)) {
                accumulated = with_left;
                code |= 1;
                l = child[1].first;
                r = child[1].second;
            } else {
                l = child[0].first;
                r = child[0].second;
            }
        }
        int sigma = matrix.alphabet_size();
        if (code >= sigma) return {sigma, accumulated};
        S with_leaf = op(accumulated, node_prod(matrix.levels, l, r));
        if (pred(with_leaf)) return {std::min(code + 1, sigma), with_leaf};
        return {code, accumulated};
    }

private:
    S node_prod(int depth, int l, int r) const {
        return segment_tree[depth].prod(l, r);
    }
};

template<class Key, class S>
using WaveletMatrixSum = WaveletMatrixGroup<
    Key, S, wavelet_matrix_detail::add<S>, wavelet_matrix_detail::zero<S>,
    wavelet_matrix_detail::negate<S>>;

template<class Key, class S>
using DynamicWaveletMatrixSum = WaveletMatrixMonoid<
    Key, S, wavelet_matrix_detail::add<S>, wavelet_matrix_detail::zero<S>>;

template<class Key, class S>
using WaveletMatrixMax = WaveletMatrixMonoid<
    Key, S, wavelet_matrix_detail::maximum<S>, wavelet_matrix_detail::lowest<S>>;

// Static 2D point set. Points are sorted by x and a wavelet matrix is built on
// y, so rectangle counting and y-order statistics take O(log(sigma_y)).
template<class X, class Y>
struct WaveletMatrix2D {
    std::vector<X> sorted_x;
    std::vector<Y> sorted_y;
    std::vector<int> sorted_index;
    WaveletMatrix<Y> matrix;

    WaveletMatrix2D() = default;
    WaveletMatrix2D(const std::vector<X>& x, const std::vector<Y>& y) { build(x, y); }

    void build(const std::vector<X>& x, const std::vector<Y>& y) {
        assert(x.size() == y.size());
        int n = (int)x.size();
        sorted_index.resize(n);
        std::iota(sorted_index.begin(), sorted_index.end(), 0);
        std::stable_sort(sorted_index.begin(), sorted_index.end(),
                         [&](int i, int j) { return x[i] < x[j]; });
        sorted_x.resize(n);
        sorted_y.resize(n);
        for (int i = 0; i < n; ++i) {
            sorted_x[i] = x[sorted_index[i]];
            sorted_y[i] = y[sorted_index[i]];
        }
        matrix.build(sorted_y);
    }

    int size() const { return matrix.size(); }

    int rectangle_freq(const X& left, const X& right, const Y& down, const Y& up) const {
        assert(!(right < left) && !(up < down));
        auto [l, r] = index_range(left, right);
        return matrix.range_freq(l, r, down, up);
    }

    Y kth_smallest_y(const X& left, const X& right, int k) const {
        assert(!(right < left));
        auto [l, r] = index_range(left, right);
        return matrix.kth_smallest(l, r, k);
    }

    Y kth_smallest_y(const X& left, const X& right,
                     const Y& down, const Y& up, int k) const {
        assert(!(right < left) && !(up < down));
        auto [l, r] = index_range(left, right);
        int offset = matrix.count_less(l, r, down);
        int count = matrix.range_freq(l, r, down, up);
        assert(0 <= k && k < count);
        return matrix.kth_smallest(l, r, offset + k);
    }

    std::pair<int, int> index_range(const X& left, const X& right) const {
        assert(!(right < left));
        int l = (int)(std::lower_bound(sorted_x.begin(), sorted_x.end(), left) - sorted_x.begin());
        int r = (int)(std::lower_bound(sorted_x.begin(), sorted_x.end(), right) - sorted_x.begin());
        return {l, r};
    }
};

// Weighted static 2D point set over an abelian group.
template<class X, class Y, class S, S (*op)(S, S), S (*e)(), S (*inverse)(S)>
struct WaveletMatrix2DGroup {
    WaveletMatrix2D<X, Y> points;
    WaveletMatrixGroup<Y, S, op, e, inverse> aggregate;

    WaveletMatrix2DGroup() = default;
    WaveletMatrix2DGroup(const std::vector<X>& x, const std::vector<Y>& y,
                         const std::vector<S>& weights) {
        build(x, y, weights);
    }

    void build(const std::vector<X>& x, const std::vector<Y>& y,
               const std::vector<S>& weights) {
        assert(x.size() == y.size() && x.size() == weights.size());
        points.build(x, y);
        std::vector<S> sorted_weight(weights.size());
        for (int i = 0; i < points.size(); ++i)
            sorted_weight[i] = weights[points.sorted_index[i]];
        aggregate.build(points.sorted_y, sorted_weight);
    }

    int size() const { return points.size(); }

    S rectangle_prod(const X& left, const X& right, const Y& down, const Y& up) const {
        assert(!(up < down));
        auto [l, r] = points.index_range(left, right);
        return aggregate.prod(l, r, down, up);
    }
};

// Weighted 2D point set over a commutative monoid. Coordinates are static;
// payloads can be assigned by the original point index.
template<class X, class Y, class S, S (*op)(S, S), S (*e)()>
struct WaveletMatrix2DMonoid {
    WaveletMatrix2D<X, Y> points;
    WaveletMatrixMonoid<Y, S, op, e> aggregate;
    std::vector<int> sorted_position;

    WaveletMatrix2DMonoid() = default;
    WaveletMatrix2DMonoid(const std::vector<X>& x, const std::vector<Y>& y,
                          const std::vector<S>& weights) {
        build(x, y, weights);
    }

    void build(const std::vector<X>& x, const std::vector<Y>& y,
               const std::vector<S>& weights) {
        assert(x.size() == y.size() && x.size() == weights.size());
        points.build(x, y);
        std::vector<S> sorted_weight(weights.size());
        sorted_position.resize(weights.size());
        for (int i = 0; i < points.size(); ++i) {
            int original = points.sorted_index[i];
            sorted_weight[i] = weights[original];
            sorted_position[original] = i;
        }
        aggregate.build(points.sorted_y, sorted_weight);
    }

    int size() const { return points.size(); }

    void set(int original_index, const S& value) {
        assert(0 <= original_index && original_index < size());
        aggregate.set(sorted_position[original_index], value);
    }

    S get(int original_index) const {
        assert(0 <= original_index && original_index < size());
        return aggregate.get(sorted_position[original_index]);
    }

    S rectangle_prod(const X& left, const X& right, const Y& down, const Y& up) const {
        assert(!(up < down));
        auto [l, r] = points.index_range(left, right);
        return aggregate.prod(l, r, down, up);
    }
};

template<class X, class Y, class S>
using WaveletMatrix2DSum = WaveletMatrix2DGroup<
    X, Y, S, wavelet_matrix_detail::add<S>, wavelet_matrix_detail::zero<S>,
    wavelet_matrix_detail::negate<S>>;

template<class X, class Y, class S>
using DynamicWaveletMatrix2DSum = WaveletMatrix2DMonoid<
    X, Y, S, wavelet_matrix_detail::add<S>, wavelet_matrix_detail::zero<S>>;

template<class X, class Y, class S>
using WaveletMatrix2DMax = WaveletMatrix2DMonoid<
    X, Y, S, wavelet_matrix_detail::maximum<S>, wavelet_matrix_detail::lowest<S>>;
