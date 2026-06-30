#pragma once
#include <bits/stdc++.h>

template<class T>
struct RangeAddRangeSum2D {
    int height, width;
    struct Fenwick2D {
        int n, m;
        std::vector<std::vector<T>> bit;
        Fenwick2D(int n_, int m_) : n(n_), m(m_), bit(n_ + 1, std::vector<T>(m_ + 1)) {}
        void add(int x, int y, T value) {
            for (int i = x + 1; i <= n; i += i & -i)
                for (int j = y + 1; j <= m; j += j & -j) bit[i][j] += value;
        }
        T sum_prefix(int x, int y) const {
            T result = 0;
            for (int i = x; i > 0; i -= i & -i)
                for (int j = y; j > 0; j -= j & -j) result += bit[i][j];
            return result;
        }
    };
    Fenwick2D b00, b10, b01, b11;

    RangeAddRangeSum2D(int height_, int width_)
        : height(height_), width(width_), b00(height_, width_), b10(height_, width_),
          b01(height_, width_), b11(height_, width_) {
        assert(height >= 0 && width >= 0);
    }
    explicit RangeAddRangeSum2D(const std::vector<std::vector<T>>& a)
        : RangeAddRangeSum2D((int)a.size(), a.empty() ? 0 : (int)a[0].size()) {
        for (int i = 0; i < height; ++i) {
            assert((int)a[i].size() == width);
            for (int j = 0; j < width; ++j) add(i, i + 1, j, j + 1, a[i][j]);
        }
    }
    void add(int row_l, int row_r, int column_l, int column_r, T value) {
        assert(0 <= row_l && row_l <= row_r && row_r <= height);
        assert(0 <= column_l && column_l <= column_r && column_r <= width);
        internal_add(row_l, column_l, value);
        internal_add(row_l, column_r, -value);
        internal_add(row_r, column_l, -value);
        internal_add(row_r, column_r, value);
    }
    T sum(int row_l, int row_r, int column_l, int column_r) const {
        assert(0 <= row_l && row_l <= row_r && row_r <= height);
        assert(0 <= column_l && column_l <= column_r && column_r <= width);
        return prefix(row_r, column_r) - prefix(row_l, column_r) - prefix(row_r, column_l) + prefix(row_l, column_l);
    }

private:
    void internal_add(int x, int y, T value) {
        if (x > height || y > width) return;
        b00.add(x, y, value);
        b10.add(x, y, value * x);
        b01.add(x, y, value * y);
        b11.add(x, y, value * x * y);
    }
    T prefix(int x, int y) const {
        T s00 = b00.sum_prefix(x, y), s10 = b10.sum_prefix(x, y);
        T s01 = b01.sum_prefix(x, y), s11 = b11.sum_prefix(x, y);
        return s00 * x * y - s10 * y - s01 * x + s11;
    }
};
