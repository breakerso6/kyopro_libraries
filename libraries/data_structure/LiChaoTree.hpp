// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/data_structure/LiChaoTree.hpp
// SPDX-License-Identifier: CC0-1.0

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
        while (true) {
            if (line.a == nodes[v].line.a) {
                if (line.b < nodes[v].line.b) nodes[v].line = line;
                return;
            }
            T m = l + (r - l) / 2;
            bool left_better = line.eval(l) < nodes[v].line.eval(l);
            bool mid_better = line.eval(m) < nodes[v].line.eval(m);
            if (mid_better) std::swap(line, nodes[v].line);
            if (r - l == 1) return;
            bool go_left = left_better != mid_better;
            // The remaining line loses at the midpoint. If it also loses at
            // both ends, it cannot improve any integer coordinate in this node.
            if (!go_left && !(line.eval(r - 1) < nodes[v].line.eval(r - 1))) return;
            int child = go_left ? nodes[v].left : nodes[v].right;
            T nl = left_better != mid_better ? l : m, nr = left_better != mid_better ? m : r;
            if (child == -1) {
                child = (int)nodes.size(); nodes.push_back({line, -1, -1});
                if (go_left) nodes[v].left = child;
                else nodes[v].right = child;
                return;
            }
            v = child; l = nl; r = nr;
        }
    }
    T query(T x, int v, T l, T r) const {
        T result = infinity;
        while (v != -1) {
            result = std::min(result, nodes[v].line.eval(x));
            if (r - l == 1) break;
            T m = l + (r - l) / 2;
            if (x < m) { v = nodes[v].left; r = m; }
            else { v = nodes[v].right; l = m; }
        }
        return result;
    }
};
