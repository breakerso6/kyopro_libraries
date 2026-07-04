#pragma once
#include <bits/stdc++.h>

template<class T>
struct CumulativeSumND {
    int dimension = 0;
    std::vector<int> shape, pref_shape;
    std::vector<long long> stride;
    std::vector<T> data;
    bool built = false;

    CumulativeSumND() = default;
    explicit CumulativeSumND(const std::vector<int>& shape_) : shape(shape_) {
        dimension = (int)shape.size();
        pref_shape.resize(dimension);
        stride.assign(dimension, 1);
        long long total = 1;
        for (int i = dimension - 1; i >= 0; --i) {
            assert(shape[i] >= 0);
            pref_shape[i] = shape[i] + 1;
            stride[i] = total;
            total *= pref_shape[i];
            assert(total <= (long long)std::numeric_limits<int>::max());
        }
        data.assign((size_t)total, T());
    }
    CumulativeSumND(const std::vector<int>& shape_, const std::vector<T>& values) : CumulativeSumND(shape_) {
        long long total_values = 1;
        for (int x : shape) total_values *= x;
        assert((long long)values.size() == total_values);
        for (long long id = 0; id < total_values; ++id) {
            long long rest = id, offset = 0;
            for (int axis = dimension - 1; axis >= 0; --axis) {
                int coord = (shape[axis] == 0 ? 0 : (int)(rest % shape[axis]));
                if (shape[axis] != 0) rest /= shape[axis];
                offset += (long long)(coord + 1) * stride[axis];
            }
            data[(size_t)offset] += values[(size_t)id];
        }
        build();
    }
    void add(const std::vector<int>& index, const T& value) {
        assert(!built);
        assert((int)index.size() == dimension);
        long long offset = 0;
        for (int axis = 0; axis < dimension; ++axis) {
            assert(0 <= index[axis] && index[axis] < shape[axis]);
            offset += (long long)(index[axis] + 1) * stride[axis];
        }
        data[(size_t)offset] += value;
    }
    void build() {
        assert(!built);
        for (int axis = 0; axis < dimension; ++axis) {
            int step = (int)stride[axis];
            int period = step * pref_shape[axis];
            for (int base = 0; base < (int)data.size(); base += period) {
                for (int coord = 1; coord < pref_shape[axis]; ++coord) {
                    int from = base + (coord - 1) * step;
                    int to = base + coord * step;
                    for (int k = 0; k < step; ++k) data[(size_t)(to + k)] += data[(size_t)(from + k)];
                }
            }
        }
        built = true;
    }
    T sum(const std::vector<int>& lower, const std::vector<int>& upper) const {
        assert(built);
        assert((int)lower.size() == dimension && (int)upper.size() == dimension);
        for (int axis = 0; axis < dimension; ++axis) {
            assert(0 <= lower[axis] && lower[axis] <= shape[axis]);
            assert(0 <= upper[axis] && upper[axis] <= shape[axis]);
            if (lower[axis] > upper[axis]) return T();
        }
        T result = T();
        assert(dimension < 62);
        for (long long mask = 0; mask < (1LL << dimension); ++mask) {
            long long offset = 0;
            int lower_count = 0;
            for (int axis = 0; axis < dimension; ++axis) {
                if (mask >> axis & 1) {
                    offset += (long long)lower[axis] * stride[axis];
                    ++lower_count;
                } else {
                    offset += (long long)upper[axis] * stride[axis];
                }
            }
            if (lower_count & 1) result -= data[(size_t)offset];
            else result += data[(size_t)offset];
        }
        return result;
    }
};
