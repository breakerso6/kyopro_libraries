#pragma once
#include <bits/stdc++.h>

template<class T, class Compare = std::less<T>>
struct CartesianTree {
    int root = -1;
    std::vector<int> parent, left, right;

    CartesianTree() = default;
    explicit CartesianTree(const std::vector<T>& a, Compare compare = Compare()) { build(a, compare); }

    void build(const std::vector<T>& a, Compare compare = Compare()) {
        int n = (int)a.size();
        parent.assign(n, -1); left.assign(n, -1); right.assign(n, -1);
        std::vector<int> stack;
        for (int i = 0; i < n; ++i) {
            int last = -1;
            while (!stack.empty() && compare(a[i], a[stack.back()])) {
                last = stack.back(); stack.pop_back();
            }
            if (!stack.empty()) { parent[i] = stack.back(); right[stack.back()] = i; }
            if (last != -1) { parent[last] = i; left[i] = last; }
            stack.push_back(i);
        }
        root = stack.empty() ? -1 : stack.front();
    }
};
