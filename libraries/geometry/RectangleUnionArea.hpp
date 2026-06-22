#pragma once
#include <bits/stdc++.h>

// Rectangles are [x1, x2) x [y1, y2). Degenerate rectangles are ignored.
template<class T = long long>
__int128_t rectangle_union_area(const std::vector<std::array<T, 4>>& rectangles) {
    struct Event { T x, y1, y2; int delta; };
    std::vector<Event> events;
    std::vector<T> ys;
    for (auto [x1, y1, x2, y2] : rectangles) if (x1 < x2 && y1 < y2) {
        events.push_back({x1, y1, y2, 1}); events.push_back({x2, y1, y2, -1});
        ys.push_back(y1); ys.push_back(y2);
    }
    if (events.empty()) return 0;
    std::sort(ys.begin(), ys.end()); ys.erase(std::unique(ys.begin(), ys.end()), ys.end());
    std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) { return a.x < b.x; });
    int segments = (int)ys.size() - 1;
    std::vector<int> cover(segments * 4 + 4);
    std::vector<__int128_t> length(segments * 4 + 4);
    std::function<void(int, int, int)> pull = [&](int v, int l, int r) {
        if (cover[v]) length[v] = (__int128_t)ys[r] - ys[l];
        else if (r - l == 1) length[v] = 0;
        else length[v] = length[v * 2] + length[v * 2 + 1];
    };
    std::function<void(int, int, int, int, int, int)> update = [&](int ql, int qr, int delta, int v, int l, int r) {
        if (qr <= l || r <= ql) return;
        if (ql <= l && r <= qr) cover[v] += delta;
        else {
            int m = (l + r) / 2;
            update(ql, qr, delta, v * 2, l, m); update(ql, qr, delta, v * 2 + 1, m, r);
        }
        pull(v, l, r);
    };
    __int128_t area = 0; T previous_x = events[0].x;
    for (int i = 0; i < (int)events.size();) {
        T x = events[i].x;
        area += length[1] * ((__int128_t)x - previous_x);
        while (i < (int)events.size() && events[i].x == x) {
            int l = std::lower_bound(ys.begin(), ys.end(), events[i].y1) - ys.begin();
            int r = std::lower_bound(ys.begin(), ys.end(), events[i].y2) - ys.begin();
            update(l, r, events[i].delta, 1, 0, segments); ++i;
        }
        previous_x = x;
    }
    return area;
}
