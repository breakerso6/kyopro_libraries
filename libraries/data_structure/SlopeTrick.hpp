#pragma once
#include <bits/stdc++.h>

template<class T = long long>
struct SlopeTrick {
    T minimum = 0, add_left = 0, add_right = 0;
    std::priority_queue<T> left;
    std::priority_queue<T, std::vector<T>, std::greater<T>> right;

    T min_f() const { return minimum; }
    std::pair<std::optional<T>, std::optional<T>> argmin() const {
        std::optional<T> l, r;
        if (!left.empty()) l = left.top() + add_left;
        if (!right.empty()) r = right.top() + add_right;
        return {l, r};
    }
    void add_constant(T value) { minimum += value; }
    void add_a_minus_x(T a) {
        if (!right.empty()) minimum += std::max<T>(0, a - (right.top() + add_right));
        push_right(a); push_left(pop_right());
    }
    void add_x_minus_a(T a) {
        if (!left.empty()) minimum += std::max<T>(0, (left.top() + add_left) - a);
        push_left(a); push_right(pop_left());
    }
    void add_abs(T a) { add_a_minus_x(a); add_x_minus_a(a); }
    void shift(T delta) { shift(delta, delta); }
    void shift(T left_delta, T right_delta) {
        assert(left_delta <= right_delta);
        add_left += left_delta; add_right += right_delta;
    }
    void clear_left() { left = {}; }
    void clear_right() { right = {}; }

private:
    void push_left(T x) { left.push(x - add_left); }
    void push_right(T x) { right.push(x - add_right); }
    T pop_left() { T x = left.top() + add_left; left.pop(); return x; }
    T pop_right() { T x = right.top() + add_right; right.pop(); return x; }
};
