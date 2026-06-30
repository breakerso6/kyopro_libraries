#pragma once
#include <bits/stdc++.h>

template<class S, class Op>
struct SegmentTree2D {
    int height = 0, width = 0, h_size = 1, w_size = 1;
    S identity;
    Op op;
    std::vector<std::vector<S>> data;

    SegmentTree2D() = default;
    SegmentTree2D(int height_, int width_, S identity_, Op op_ = Op())
        : height(height_), width(width_), identity(identity_), op(op_) {
        while (h_size < std::max(1, height)) h_size <<= 1;
        while (w_size < std::max(1, width)) w_size <<= 1;
        data.assign(2 * h_size, std::vector<S>(2 * w_size, identity));
    }
    explicit SegmentTree2D(const std::vector<std::vector<S>>& a, S identity_, Op op_ = Op())
        : SegmentTree2D((int)a.size(), a.empty() ? 0 : (int)a[0].size(), identity_, op_) {
        for (int i = 0; i < height; ++i) {
            assert((int)a[i].size() == width);
            for (int j = 0; j < width; ++j) data[i + h_size][j + w_size] = a[i][j];
            for (int j = w_size - 1; j > 0; --j) data[i + h_size][j] = op(data[i + h_size][j << 1], data[i + h_size][j << 1 | 1]);
        }
        for (int i = h_size - 1; i > 0; --i)
            for (int j = 1; j < 2 * w_size; ++j) data[i][j] = op(data[i << 1][j], data[i << 1 | 1][j]);
    }
    void set(int row, int column, const S& value) {
        assert(0 <= row && row < height && 0 <= column && column < width);
        int x = row + h_size, y = column + w_size;
        data[x][y] = value;
        for (int j = y >> 1; j > 0; j >>= 1) data[x][j] = op(data[x][j << 1], data[x][j << 1 | 1]);
        for (x >>= 1; x > 0; x >>= 1) {
            data[x][y] = op(data[x << 1][y], data[x << 1 | 1][y]);
            for (int j = y >> 1; j > 0; j >>= 1) data[x][j] = op(data[x][j << 1], data[x][j << 1 | 1]);
        }
    }
    S prod(int row_l, int row_r, int column_l, int column_r) const {
        assert(0 <= row_l && row_l <= row_r && row_r <= height);
        assert(0 <= column_l && column_l <= column_r && column_r <= width);
        S left_result = identity, right_result = identity;
        for (int l = row_l + h_size, r = row_r + h_size; l < r; l >>= 1, r >>= 1) {
            if (l & 1) left_result = op(left_result, prod_y(l++, column_l, column_r));
            if (r & 1) right_result = op(prod_y(--r, column_l, column_r), right_result);
        }
        return op(left_result, right_result);
    }

private:
    S prod_y(int x, int column_l, int column_r) const {
        S left_result = identity, right_result = identity;
        for (int l = column_l + w_size, r = column_r + w_size; l < r; l >>= 1, r >>= 1) {
            if (l & 1) left_result = op(left_result, data[x][l++]);
            if (r & 1) right_result = op(data[x][--r], right_result);
        }
        return op(left_result, right_result);
    }
};
