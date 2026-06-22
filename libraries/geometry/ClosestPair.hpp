#pragma once
#include <bits/stdc++.h>
#include "libraries/geometry/GeometryInt.hpp"

namespace geometry_int {
template<class T>
i128 closest_pair_distance2(std::vector<Point<T>> points) {
    if (points.size() < 2) return -1;
    std::sort(points.begin(), points.end());
    std::vector<Point<T>> buffer(points.size());
    const i128 INF = (i128(1) << 126);
    std::function<i128(int, int)> solve = [&](int l, int r) -> i128 {
        if (r - l <= 3) {
            i128 answer = INF;
            for (int i = l; i < r; ++i) for (int j = i + 1; j < r; ++j) answer = std::min(answer, distance2(points[i], points[j]));
            std::sort(points.begin() + l, points.begin() + r, [](auto a, auto b) { return a.y != b.y ? a.y < b.y : a.x < b.x; });
            return answer;
        }
        int m = (l + r) / 2; T middle_x = points[m].x;
        i128 answer = std::min(solve(l, m), solve(m, r));
        std::merge(points.begin() + l, points.begin() + m, points.begin() + m, points.begin() + r,
                   buffer.begin(), [](auto a, auto b) { return a.y != b.y ? a.y < b.y : a.x < b.x; });
        std::copy(buffer.begin(), buffer.begin() + (r - l), points.begin() + l);
        std::vector<Point<T>> strip;
        for (int i = l; i < r; ++i) {
            i128 dx = (i128)points[i].x - middle_x;
            if (dx * dx >= answer) continue;
            for (int j = (int)strip.size() - 1; j >= 0; --j) {
                i128 dy = (i128)points[i].y - strip[j].y;
                if (dy * dy >= answer) break;
                answer = std::min(answer, distance2(points[i], strip[j]));
            }
            strip.push_back(points[i]);
        }
        return answer;
    };
    return solve(0, (int)points.size());
}
}  // namespace geometry_int
