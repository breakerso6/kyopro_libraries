#pragma once
#include <bits/stdc++.h>

template<class T>
struct LiChaoTree {
    struct Line { T a, b; T eval(T x) const { return a * x + b; } };
    struct Node { Line line; int left = -1, right = -1; };
    T low, high, infinity;
    std::vector<Node> nodes;

    LiChaoTree(T low_, T high_, T infinity_) : low(low_), high(high_), infinity(infinity_) {
        assert(low < high);
        nodes.push_back({{0, infinity}, -1, -1});
    }
    void add_line(T a, T b) { add_line({a, b}, 0, low, high); }
    T min(T x) const { assert(low <= x && x < high); return query(x, 0, low, high); }

private:
    void add_line(Line line, int v, T l, T r) {
        T m = l + (r - l) / 2;
        bool left_better = line.eval(l) < nodes[v].line.eval(l);
        bool mid_better = line.eval(m) < nodes[v].line.eval(m);
        if (mid_better) std::swap(line, nodes[v].line);
        if (r - l == 1) return;
        bool go_left = left_better != mid_better;
        int child = go_left ? nodes[v].left : nodes[v].right;
        T nl = left_better != mid_better ? l : m, nr = left_better != mid_better ? m : r;
        if (child == -1) {
            child = (int)nodes.size(); nodes.push_back({line, -1, -1});
            if (go_left) nodes[v].left = child;
            else nodes[v].right = child;
        }
        else add_line(line, child, nl, nr);
    }
    T query(T x, int v, T l, T r) const {
        T result = nodes[v].line.eval(x);
        if (r - l == 1) return result;
        T m = l + (r - l) / 2;
        int child = x < m ? nodes[v].left : nodes[v].right;
        if (child == -1) return result;
        return std::min(result, query(x, child, x < m ? l : m, x < m ? m : r));
    }
};
