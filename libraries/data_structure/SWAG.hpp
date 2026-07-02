#pragma once
#include <bits/stdc++.h>

template<class S, S (*op)(S, S), S (*e)()>
struct SWAG {
    std::vector<std::pair<S, S>> front, back;

    SWAG() = default;
    int size() const { return (int)front.size() + (int)back.size(); }
    bool empty() const { return size() == 0; }
    void push(const S& x) {
        S agg = back.empty() ? x : op(back.back().second, x);
        back.push_back({x, agg});
    }
    void pop() {
        assert(!empty());
        if (front.empty()) {
            while (!back.empty()) {
                S x = back.back().first; back.pop_back();
                S agg = front.empty() ? x : op(x, front.back().second);
                front.push_back({x, agg});
            }
        }
        front.pop_back();
    }
    S prod() const {
        if (front.empty() && back.empty()) return e();
        if (front.empty()) return back.back().second;
        if (back.empty()) return front.back().second;
        return op(front.back().second, back.back().second);
    }
};
