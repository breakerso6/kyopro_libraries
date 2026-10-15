// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/geometry/RectangleUnionArea.hpp
// SPDX-License-Identifier: CC0-1.0

#pragma once
#include <bits/stdc++.h>

// Rectangles are [x1, x2) x [y1, y2). Degenerate rectangles are ignored.
template<class T = long long>
__int128_t rectangle_union_area(const std::vector<std::array<T, 4>>& rectangles) {
    struct Event { T x; int left, right, delta; };
    std::vector<Event> events;
    std::vector<T> ys;
    events.reserve(2 * rectangles.size());
    ys.reserve(2 * rectangles.size());
    for (auto [x1, y1, x2, y2] : rectangles) if (x1 < x2 && y1 < y2) {
        ys.push_back(y1); ys.push_back(y2);
    }
    if (ys.empty()) return 0;
    std::sort(ys.begin(), ys.end()); ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
    // Both x-events share the same compressed y-interval. Search once per
    // rectangle and sort smaller events containing indices instead of T.
    for (auto [x1, y1, x2, y2] : rectangles) if (x1 < x2 && y1 < y2) {
        int l = std::lower_bound(ys.begin(), ys.end(), y1) - ys.begin();
        int r = std::lower_bound(ys.begin(), ys.end(), y2) - ys.begin();
        events.push_back({x1, l, r, 1}); events.push_back({x2, l, r, -1});
    }
    std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) { return a.x < b.x; });
    int segments = (int)ys.size() - 1;
    std::vector<int> cover(segments * 4 + 4);
    std::vector<__int128_t> length(segments * 4 + 4);
    auto update = [&](auto&& self, int ql, int qr, int delta, int v, int l, int r) -> void {
        if (ql <= l && r <= qr) cover[v] += delta;
        else {
            int m = (l + r) / 2;
            if (ql < m) self(self, ql, qr, delta, v * 2, l, m);
            if (m < qr) self(self, ql, qr, delta, v * 2 + 1, m, r);
        }
        if (cover[v]) length[v] = (__int128_t)ys[r] - ys[l];
        else if (r - l == 1) length[v] = 0;
        else length[v] = length[v * 2] + length[v * 2 + 1];
    };
    __int128_t area = 0; T previous_x = events[0].x;
    for (int i = 0; i < (int)events.size();) {
        T x = events[i].x;
        area += length[1] * ((__int128_t)x - previous_x);
        while (i < (int)events.size() && events[i].x == x) {
            update(update, events[i].left, events[i].right, events[i].delta, 1, 0, segments); ++i;
        }
        previous_x = x;
    }
    return area;
}
