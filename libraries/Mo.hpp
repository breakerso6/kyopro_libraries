#pragma once
#include <bits/stdc++.h>

struct Mo {
    int n;
    std::vector<int> left, right;
    explicit Mo(int n_) : n(n_) {}
    int add_query(int l, int r) { left.push_back(l); right.push_back(r); return (int)left.size() - 1; }

    template<class AddLeft, class AddRight, class EraseLeft, class EraseRight, class Answer>
    void run(AddLeft add_left, AddRight add_right, EraseLeft erase_left, EraseRight erase_right, Answer answer) const {
        int q = (int)left.size();
        int block = std::max(1, (int)(n / std::max(1.0, std::sqrt((double)q))));
        std::vector<int> order(q); std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int a, int b) {
            int ab = left[a] / block, bb = left[b] / block;
            if (ab != bb) return ab < bb;
            return ab & 1 ? right[a] > right[b] : right[a] < right[b];
        });
        int l = 0, r = 0;
        for (int id : order) {
            while (left[id] < l) add_left(--l);
            while (r < right[id]) add_right(r++);
            while (l < left[id]) erase_left(l++);
            while (right[id] < r) erase_right(--r);
            answer(id);
        }
    }
};
