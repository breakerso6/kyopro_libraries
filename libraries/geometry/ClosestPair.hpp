// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/geometry/ClosestPair.hpp

#pragma once
#include <bits/stdc++.h>
#include "libraries/geometry/GeometryInt.hpp"

namespace geometry_int {
template<class T>
i128 closest_pair_distance2(std::vector<Point<T>> points) {
    if (points.size() < 2) return -1;
    std::sort(points.begin(), points.end());
    for (size_t i = 1; i < points.size(); ++i)
        if (points[i] == points[i - 1]) return 0;
    std::vector<Point<T>> buffer(points.size());
    const i128 INF = (i128(1) << 126);
    auto solve = [&](auto&& self, int l, int r) -> i128 {
        if (r - l <= 3) {
            i128 answer = INF;
            for (int i = l; i < r; ++i) for (int j = i + 1; j < r; ++j) answer = std::min(answer, distance2(points[i], points[j]));
            std::sort(points.begin() + l, points.begin() + r, [](auto a, auto b) { return a.y != b.y ? a.y < b.y : a.x < b.x; });
            return answer;
        }
        int m = (l + r) / 2; T middle_x = points[m].x;
        i128 answer = std::min(self(self, l, m), self(self, m, r));
        std::merge(points.begin() + l, points.begin() + m, points.begin() + m, points.begin() + r,
                   buffer.begin(), [](auto a, auto b) { return a.y != b.y ? a.y < b.y : a.x < b.x; });
        std::copy(buffer.begin(), buffer.begin() + (r - l), points.begin() + l);
        // The merge scratch space is no longer needed, so reuse it as the
        // strip instead of allocating another vector at every recursion.
        int strip_size = 0;
        for (int i = l; i < r; ++i) {
            i128 dx = (i128)points[i].x - middle_x;
            if (dx * dx >= answer) continue;
            for (int j = strip_size - 1; j >= 0; --j) {
                i128 dy = (i128)points[i].y - buffer[j].y;
                if (dy * dy >= answer) break;
                answer = std::min(answer, distance2(points[i], buffer[j]));
            }
            buffer[strip_size++] = points[i];
        }
        return answer;
    };
    return solve(solve, 0, (int)points.size());
}
}  // namespace geometry_int
