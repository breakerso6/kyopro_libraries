#pragma once
#include <bits/stdc++.h>

template<class T>
struct ConvexHullTrick {
    struct Line { T a, b; T eval(T x) const { return a * x + b; } };
    std::vector<Line> hull;
    std::vector<long double> left_x;
    bool minimum;

    explicit ConvexHullTrick(bool minimum_ = true) : minimum(minimum_) {}
    // For minimum queries, add lines in decreasing slope order. For maximum queries, add increasing slopes.
    // Query is O(log N).
    void add_line(T a, T b) {
        if (!minimum) a = -a, b = -b;
        Line line{a, b};
        if (!hull.empty() && hull.back().a == line.a) {
            if (hull.back().b <= line.b) return;
            hull.pop_back();
            left_x.pop_back();
        }
        long double lx = -std::numeric_limits<long double>::infinity();
        while (!hull.empty()) {
            lx = intersect_x(hull.back(), line);
            if (lx > left_x.back()) break;
            hull.pop_back();
            left_x.pop_back();
        }
        hull.push_back(line);
        left_x.push_back(hull.size() == 1 ? -std::numeric_limits<long double>::infinity() : lx);
    }
    T query(T x) const {
        assert(!hull.empty());
        int id = (int)(std::upper_bound(left_x.begin(), left_x.end(), (long double)x) - left_x.begin()) - 1;
        T value = hull[id].eval(x);
        return minimum ? value : -value;
    }

private:
    static long double intersect_x(const Line& l, const Line& r) {
        return (long double)(r.b - l.b) / (l.a - r.a);
    }
};
