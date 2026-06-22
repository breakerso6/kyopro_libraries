#pragma once
#include <bits/stdc++.h>

namespace geometry_int {
using i128 = __int128_t;

template<class T = long long>
struct Point {
    T x{}, y{};
    Point operator+(const Point& p) const { return {x + p.x, y + p.y}; }
    Point operator-(const Point& p) const { return {x - p.x, y - p.y}; }
    bool operator==(const Point& p) const { return x == p.x && y == p.y; }
    bool operator!=(const Point& p) const { return !(*this == p); }
    bool operator<(const Point& p) const { return x != p.x ? x < p.x : y < p.y; }
};

template<class T> i128 dot(Point<T> a, Point<T> b) { return (i128)a.x * b.x + (i128)a.y * b.y; }
template<class T> i128 cross(Point<T> a, Point<T> b) { return (i128)a.x * b.y - (i128)a.y * b.x; }
template<class T> i128 cross(Point<T> a, Point<T> b, Point<T> c) {
    return ((i128)b.x - a.x) * ((i128)c.y - a.y) - ((i128)b.y - a.y) * ((i128)c.x - a.x);
}
template<class T> i128 norm2(Point<T> p) { return dot(p, p); }
template<class T> i128 distance2(Point<T> a, Point<T> b) {
    i128 dx = (i128)a.x - b.x, dy = (i128)a.y - b.y;
    return dx * dx + dy * dy;
}
template<class T> i128 cross_vectors(Point<T> a, Point<T> b, Point<T> c, Point<T> d) {
    return ((i128)b.x - a.x) * ((i128)d.y - c.y) - ((i128)b.y - a.y) * ((i128)d.x - c.x);
}

template<class T>
int ccw(Point<T> a, Point<T> b, Point<T> c) {
    i128 value = cross(a, b, c);
    return (value > 0) - (value < 0);
}
template<class T>
bool on_segment(Point<T> a, Point<T> b, Point<T> p) {
    i128 pax = (i128)p.x - a.x, pay = (i128)p.y - a.y;
    i128 pbx = (i128)p.x - b.x, pby = (i128)p.y - b.y;
    return cross(a, b, p) == 0 && pax * pbx + pay * pby <= 0;
}
template<class T>
bool segments_intersect(Point<T> a, Point<T> b, Point<T> c, Point<T> d) {
    int ab_c = ccw(a, b, c), ab_d = ccw(a, b, d);
    int cd_a = ccw(c, d, a), cd_b = ccw(c, d, b);
    if (!ab_c && on_segment(a, b, c)) return true;
    if (!ab_d && on_segment(a, b, d)) return true;
    if (!cd_a && on_segment(c, d, a)) return true;
    if (!cd_b && on_segment(c, d, b)) return true;
    return ab_c * ab_d < 0 && cd_a * cd_b < 0;
}

// Removes duplicate points and collinear points inside hull edges.
template<class T>
std::vector<Point<T>> convex_hull(std::vector<Point<T>> points) {
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    if (points.size() <= 1) return points;
    std::vector<Point<T>> lower, upper;
    for (auto p : points) {
        while (lower.size() >= 2 && cross(lower[lower.size() - 2], lower.back(), p) <= 0) lower.pop_back();
        lower.push_back(p);
    }
    for (auto it = points.rbegin(); it != points.rend(); ++it) {
        while (upper.size() >= 2 && cross(upper[upper.size() - 2], upper.back(), *it) <= 0) upper.pop_back();
        upper.push_back(*it);
    }
    lower.pop_back(); upper.pop_back();
    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}

template<class T>
i128 polygon_area2(const std::vector<Point<T>>& polygon) {
    i128 result = 0;
    for (int i = 0, n = (int)polygon.size(); i < n; ++i) result += cross(polygon[i], polygon[(i + 1) % n]);
    return result < 0 ? -result : result;
}

// -1: outside, 0: boundary, 1: inside. Polygon may be clockwise.
template<class T>
int point_in_polygon(const std::vector<Point<T>>& polygon, Point<T> p) {
    bool inside = false;
    for (int i = 0, n = (int)polygon.size(); i < n; ++i) {
        Point<T> a = polygon[i], b = polygon[(i + 1) % n];
        if (on_segment(a, b, p)) return 0;
        if ((a.y > p.y) != (b.y > p.y)) {
            i128 value = cross(b - a, p - a);
            if ((value > 0) == (b.y > a.y)) inside = !inside;
        }
    }
    return inside ? 1 : -1;
}

template<class T>
i128 convex_diameter2(const std::vector<Point<T>>& polygon) {
    int n = (int)polygon.size();
    if (n <= 1) return 0;
    if (n == 2) return distance2(polygon[0], polygon[1]);
    i128 answer = 0; int j = 1;
    for (int i = 0; i < n; ++i) {
        int ni = (i + 1) % n;
        while (cross_vectors(polygon[i], polygon[ni], polygon[j], polygon[(j + 1) % n]) > 0) j = (j + 1) % n;
        answer = std::max(answer, distance2(polygon[i], polygon[j]));
        answer = std::max(answer, distance2(polygon[ni], polygon[j]));
    }
    return answer;
}

inline i128 gcd128(i128 a, i128 b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) { i128 remainder = a % b; a = b; b = remainder; }
    return a;
}
template<class T>
i128 lattice_points_on_boundary(const std::vector<Point<T>>& polygon) {
    i128 result = 0;
    for (int i = 0, n = (int)polygon.size(); i < n; ++i) {
        i128 dx = (i128)polygon[(i + 1) % n].x - polygon[i].x;
        i128 dy = (i128)polygon[(i + 1) % n].y - polygon[i].y;
        result += gcd128(dx, dy);
    }
    return result;
}
}  // namespace geometry_int
