#pragma once
#include <bits/stdc++.h>

namespace geometry_real {
using Real = long double;
constexpr Real EPS = 1e-12L;
constexpr Real PI = 3.141592653589793238462643383279502884L;

struct Point {
    Real x{}, y{};
    Point operator+(Point p) const { return {x + p.x, y + p.y}; }
    Point operator-(Point p) const { return {x - p.x, y - p.y}; }
    Point operator*(Real k) const { return {x * k, y * k}; }
    Point operator/(Real k) const { return {x / k, y / k}; }
};
struct Line { Point a, b; };
struct Circle { Point center; Real radius; };

inline Real dot(Point a, Point b) { return a.x * b.x + a.y * b.y; }
inline Real cross(Point a, Point b) { return a.x * b.y - a.y * b.x; }
inline Real norm2(Point p) { return dot(p, p); }
inline Real abs(Point p) { return std::sqrt(norm2(p)); }
inline Point rotate(Point p, Real angle) {
    Real c = std::cos(angle), s = std::sin(angle);
    return {p.x * c - p.y * s, p.x * s + p.y * c};
}
inline Point projection(Line l, Point p) {
    Point d = l.b - l.a;
    return l.a + d * (dot(p - l.a, d) / norm2(d));
}
inline Point reflection(Line l, Point p) { return projection(l, p) * 2 - p; }
inline Real distance_line_point(Line l, Point p) { return std::abs(cross(l.b - l.a, p - l.a)) / abs(l.b - l.a); }
inline Real distance_segment_point(Line s, Point p) {
    if (dot(s.b - s.a, p - s.a) <= 0) return abs(p - s.a);
    if (dot(s.a - s.b, p - s.b) <= 0) return abs(p - s.b);
    return distance_line_point(s, p);
}
inline int sign(Real x) { return (x > EPS) - (x < -EPS); }
inline int ccw(Point a, Point b, Point c) { return sign(cross(b - a, c - a)); }
inline bool on_segment(Point a, Point b, Point p) {
    return std::abs(cross(b - a, p - a)) <= EPS && dot(p - a, p - b) <= EPS;
}
inline bool segments_intersect(Line s, Line t) {
    int s1 = ccw(s.a, s.b, t.a), s2 = ccw(s.a, s.b, t.b);
    int t1 = ccw(t.a, t.b, s.a), t2 = ccw(t.a, t.b, s.b);
    if (!s1 && on_segment(s.a, s.b, t.a)) return true;
    if (!s2 && on_segment(s.a, s.b, t.b)) return true;
    if (!t1 && on_segment(t.a, t.b, s.a)) return true;
    if (!t2 && on_segment(t.a, t.b, s.b)) return true;
    return s1 * s2 < 0 && t1 * t2 < 0;
}
inline Real distance_segments(Line s, Line t) {
    if (segments_intersect(s, t)) return 0;
    return std::min({distance_segment_point(s, t.a), distance_segment_point(s, t.b),
                     distance_segment_point(t, s.a), distance_segment_point(t, s.b)});
}

// Returns nullopt for parallel or coincident lines.
inline std::optional<Point> line_intersection(Line l, Line m) {
    Point ld = l.b - l.a, md = m.b - m.a;
    Real denominator = cross(ld, md);
    if (std::abs(denominator) <= EPS) return std::nullopt;
    return l.a + ld * (cross(m.a - l.a, md) / denominator);
}
inline std::vector<Point> circle_line_intersections(Circle c, Line l) {
    Point foot = projection(l, c.center), unit = (l.b - l.a) / abs(l.b - l.a);
    Real h2 = c.radius * c.radius - norm2(foot - c.center);
    if (h2 < -EPS) return {};
    if (h2 <= EPS) return {foot};
    Point offset = unit * std::sqrt(std::max<Real>(0, h2));
    return {foot - offset, foot + offset};
}
inline std::vector<Point> circle_intersections(Circle a, Circle b) {
    Point d = b.center - a.center; Real distance = abs(d);
    if (distance <= EPS) return {};
    if (distance > a.radius + b.radius + EPS || distance < std::abs(a.radius - b.radius) - EPS) return {};
    Real x = (a.radius * a.radius - b.radius * b.radius + distance * distance) / (2 * distance);
    Real h2 = a.radius * a.radius - x * x;
    Point base = a.center + d * (x / distance);
    if (h2 <= EPS) return {base};
    Point perpendicular{-d.y / distance, d.x / distance};
    Point offset = perpendicular * std::sqrt(std::max<Real>(0, h2));
    return {base - offset, base + offset};
}
}  // namespace geometry_real
