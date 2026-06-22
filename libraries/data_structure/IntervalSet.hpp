#pragma once
#include <bits/stdc++.h>

// Maintains a union of disjoint half-open intervals [l, r).
template<class T>
struct IntervalSet {
    std::map<T, T> intervals;

    void insert(T l, T r) {
        if (!(l < r)) return;
        auto it = intervals.lower_bound(l);
        if (it != intervals.begin() && std::prev(it)->second >= l) --it;
        while (it != intervals.end() && it->first <= r) {
            if (it->second < l) { ++it; continue; }
            l = std::min(l, it->first); r = std::max(r, it->second);
            it = intervals.erase(it);
        }
        intervals[l] = r;
    }
    void erase(T l, T r) {
        if (!(l < r)) return;
        auto it = intervals.upper_bound(l);
        if (it != intervals.begin()) --it;
        std::vector<std::pair<T, T>> remaining;
        while (it != intervals.end()) {
            auto [a, b] = *it;
            if (b <= l) { ++it; continue; }
            if (r <= a) break;
            it = intervals.erase(it);
            if (a < l) remaining.push_back({a, l});
            if (r < b) remaining.push_back({r, b});
        }
        for (auto [a, b] : remaining) intervals[a] = b;
    }
    bool contains(T x) const {
        auto it = intervals.upper_bound(x);
        return it != intervals.begin() && x < std::prev(it)->second;
    }
    bool covers(T l, T r) const {
        if (!(l < r)) return true;
        auto it = intervals.upper_bound(l);
        return it != intervals.begin() && r <= std::prev(it)->second;
    }
    T mex(T x) const {
        auto it = intervals.upper_bound(x);
        if (it != intervals.begin() && x < std::prev(it)->second) return std::prev(it)->second;
        return x;
    }
};
