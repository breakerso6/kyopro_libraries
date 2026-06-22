#pragma once
#include <bits/stdc++.h>

struct SegmentTreeBeats {
    using i64 = long long;
    static constexpr i64 INF = std::numeric_limits<i64>::max() / 4;
    struct Node {
        i64 max1 = -INF, max2 = -INF, min1 = INF, min2 = INF;
        i64 sum = 0, add = 0;
        int max_count = 0, min_count = 0, length = 0;
    };
    int n = 1, size_ = 0;
    std::vector<Node> tree;

    SegmentTreeBeats() = default;
    explicit SegmentTreeBeats(const std::vector<i64>& a) { build(a); }

    void build(const std::vector<i64>& a) {
        size_ = (int)a.size(); n = 1; while (n < size_) n <<= 1;
        tree.assign(2 * n, Node());
        for (int i = 0; i < n; ++i) {
            Node& x = tree[n + i]; x.length = 1;
            i64 value = i < size_ ? a[i] : 0;
            x.max1 = x.min1 = x.sum = value; x.max_count = x.min_count = 1;
        }
        for (int i = n - 1; i; --i) pull(i);
    }
    int size() const { return size_; }
    void range_chmin(int l, int r, i64 x) { range_chmin(l, r, x, 1, 0, n); }
    void range_chmax(int l, int r, i64 x) { range_chmax(l, r, x, 1, 0, n); }
    void range_add(int l, int r, i64 x) { range_add(l, r, x, 1, 0, n); }
    i64 range_sum(int l, int r) { return range_sum(l, r, 1, 0, n); }
    i64 range_min(int l, int r) { return range_min(l, r, 1, 0, n); }
    i64 range_max(int l, int r) { return range_max(l, r, 1, 0, n); }

private:
    void pull(int k) {
        const Node &a = tree[2 * k], &b = tree[2 * k + 1]; Node& x = tree[k];
        x.length = a.length + b.length; x.sum = a.sum + b.sum; x.add = 0;
        if (a.max1 > b.max1) { x.max1 = a.max1; x.max_count = a.max_count; x.max2 = std::max(a.max2, b.max1); }
        else if (a.max1 < b.max1) { x.max1 = b.max1; x.max_count = b.max_count; x.max2 = std::max(a.max1, b.max2); }
        else { x.max1 = a.max1; x.max_count = a.max_count + b.max_count; x.max2 = std::max(a.max2, b.max2); }
        if (a.min1 < b.min1) { x.min1 = a.min1; x.min_count = a.min_count; x.min2 = std::min(a.min2, b.min1); }
        else if (a.min1 > b.min1) { x.min1 = b.min1; x.min_count = b.min_count; x.min2 = std::min(a.min1, b.min2); }
        else { x.min1 = a.min1; x.min_count = a.min_count + b.min_count; x.min2 = std::min(a.min2, b.min2); }
    }
    void apply_add(int k, i64 x) {
        Node& a = tree[k]; a.sum += x * a.length; a.max1 += x; a.min1 += x;
        if (a.max2 != -INF) a.max2 += x;
        if (a.min2 != INF) a.min2 += x;
        a.add += x;
    }
    void apply_chmin(int k, i64 x) {
        Node& a = tree[k]; if (a.max1 <= x) return;
        a.sum += (x - a.max1) * a.max_count;
        if (a.min1 == a.max1) a.min1 = x;
        else if (a.min2 == a.max1) a.min2 = x;
        a.max1 = x;
    }
    void apply_chmax(int k, i64 x) {
        Node& a = tree[k]; if (a.min1 >= x) return;
        a.sum += (x - a.min1) * a.min_count;
        if (a.max1 == a.min1) a.max1 = x;
        else if (a.max2 == a.min1) a.max2 = x;
        a.min1 = x;
    }
    void push(int k) {
        if (k >= n) return;
        if (tree[k].add != 0) { apply_add(2 * k, tree[k].add); apply_add(2 * k + 1, tree[k].add); tree[k].add = 0; }
        apply_chmin(2 * k, tree[k].max1); apply_chmin(2 * k + 1, tree[k].max1);
        apply_chmax(2 * k, tree[k].min1); apply_chmax(2 * k + 1, tree[k].min1);
    }
    void range_chmin(int l, int r, i64 x, int k, int a, int b) {
        if (b <= l || r <= a || tree[k].max1 <= x) return;
        if (l <= a && b <= r && tree[k].max2 < x) { apply_chmin(k, x); return; }
        push(k); int m = (a + b) / 2;
        range_chmin(l, r, x, 2 * k, a, m); range_chmin(l, r, x, 2 * k + 1, m, b); pull(k);
    }
    void range_chmax(int l, int r, i64 x, int k, int a, int b) {
        if (b <= l || r <= a || tree[k].min1 >= x) return;
        if (l <= a && b <= r && x < tree[k].min2) { apply_chmax(k, x); return; }
        push(k); int m = (a + b) / 2;
        range_chmax(l, r, x, 2 * k, a, m); range_chmax(l, r, x, 2 * k + 1, m, b); pull(k);
    }
    void range_add(int l, int r, i64 x, int k, int a, int b) {
        if (b <= l || r <= a) return;
        if (l <= a && b <= r) { apply_add(k, x); return; }
        push(k); int m = (a + b) / 2;
        range_add(l, r, x, 2 * k, a, m); range_add(l, r, x, 2 * k + 1, m, b); pull(k);
    }
    i64 range_sum(int l, int r, int k, int a, int b) {
        if (b <= l || r <= a) return 0;
        if (l <= a && b <= r) return tree[k].sum;
        push(k); int m = (a + b) / 2; return range_sum(l, r, 2 * k, a, m) + range_sum(l, r, 2 * k + 1, m, b);
    }
    i64 range_min(int l, int r, int k, int a, int b) {
        if (b <= l || r <= a) return INF;
        if (l <= a && b <= r) return tree[k].min1;
        push(k); int m = (a + b) / 2; return std::min(range_min(l, r, 2 * k, a, m), range_min(l, r, 2 * k + 1, m, b));
    }
    i64 range_max(int l, int r, int k, int a, int b) {
        if (b <= l || r <= a) return -INF;
        if (l <= a && b <= r) return tree[k].max1;
        push(k); int m = (a + b) / 2; return std::max(range_max(l, r, 2 * k, a, m), range_max(l, r, 2 * k + 1, m, b));
    }
};
