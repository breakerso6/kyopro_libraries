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
struct Segment { Point a, b; };
// Represents the circumference. radius must be nonnegative.
struct Circle { Point center; Real radius; };

enum class LineRelation { Intersecting, Parallel, Coincident };
enum class SegmentRelation { Disjoint, Touching, Proper, Overlapping };
enum class CircleRelation {
    Separate,
    ExternallyTangent,
    Intersecting,
    InternallyTangent,
    Contained,
    Coincident
};

inline Real dot(Point a, Point b) { return a.x * b.x + a.y * b.y; }
inline Real cross(Point a, Point b) { return a.x * b.y - a.y * b.x; }
inline Real norm2(Point p) { return dot(p, p); }
inline Real abs(Point p) { return std::sqrt(norm2(p)); }
inline int sign(Real x) { return (x > EPS) - (x < -EPS); }
inline bool same_point(Point a, Point b) { return abs(a - b) <= EPS; }
inline Point direction(Line line) { return line.b - line.a; }
inline Point direction(Segment segment) { return segment.b - segment.a; }
inline bool is_degenerate(Line line) { return abs(direction(line)) <= EPS; }
inline bool is_degenerate(Segment segment) { return abs(direction(segment)) <= EPS; }
inline bool directions_parallel(Point a, Point b) {
    return abs(a) > EPS && abs(b) > EPS && sign(cross(a, b)) == 0;
}
inline bool directions_perpendicular(Point a, Point b) {
    return abs(a) > EPS && abs(b) > EPS && sign(dot(a, b)) == 0;
}
inline bool is_parallel(Line a, Line b) { return directions_parallel(direction(a), direction(b)); }
inline bool is_parallel(Line a, Segment b) { return directions_parallel(direction(a), direction(b)); }
inline bool is_parallel(Segment a, Line b) { return is_parallel(b, a); }
inline bool is_parallel(Segment a, Segment b) { return directions_parallel(direction(a), direction(b)); }
inline bool is_perpendicular(Line a, Line b) { return directions_perpendicular(direction(a), direction(b)); }
inline bool is_perpendicular(Line a, Segment b) { return directions_perpendicular(direction(a), direction(b)); }
inline bool is_perpendicular(Segment a, Line b) { return is_perpendicular(b, a); }
inline bool is_perpendicular(Segment a, Segment b) { return directions_perpendicular(direction(a), direction(b)); }
inline Point rotate(Point p, Real angle) {
    Real c = std::cos(angle), s = std::sin(angle);
    return {p.x * c - p.y * s, p.x * s + p.y * c};
}
inline Point projection(Line l, Point p) {
    assert(!is_degenerate(l));
    Point d = direction(l);
    return l.a + d * (dot(p - l.a, d) / norm2(d));
}
inline Point reflection(Line l, Point p) { return projection(l, p) * 2 - p; }
inline Real distance_line_point(Line l, Point p) {
    assert(!is_degenerate(l));
    return std::abs(cross(direction(l), p - l.a)) / abs(direction(l));
}
inline Real distance_segment_point(Segment s, Point p) {
    if (is_degenerate(s)) return abs(p - s.a);
    if (dot(s.b - s.a, p - s.a) <= 0) return abs(p - s.a);
    if (dot(s.a - s.b, p - s.b) <= 0) return abs(p - s.b);
    return distance_line_point(Line{s.a, s.b}, p);
}
template<class L, std::enable_if_t<std::is_same_v<std::decay_t<L>, Line>, int> = 0>
inline Real distance_segment_point(const L& s, Point p) {
    return distance_segment_point(Segment{s.a, s.b}, p);
}
inline int ccw(Point a, Point b, Point c) { return sign(cross(b - a, c - a)); }
inline bool on_segment(Point a, Point b, Point p) {
    return std::abs(cross(b - a, p - a)) <= EPS && dot(p - a, p - b) <= EPS;
}
inline bool on_line(Line line, Point p) {
    assert(!is_degenerate(line));
    return sign(cross(direction(line), p - line.a)) == 0;
}
inline bool on_segment(Segment segment, Point p) { return on_segment(segment.a, segment.b, p); }

inline LineRelation line_relation(Line a, Line b) {
    assert(!is_degenerate(a) && !is_degenerate(b));
    if (!is_parallel(a, b)) return LineRelation::Intersecting;
    return on_line(a, b.a) ? LineRelation::Coincident : LineRelation::Parallel;
}

inline SegmentRelation segment_relation(Segment s, Segment t) {
    if (is_degenerate(s) && is_degenerate(t)) {
        return same_point(s.a, t.a) ? SegmentRelation::Touching : SegmentRelation::Disjoint;
    }
    if (is_degenerate(s)) return on_segment(t, s.a) ? SegmentRelation::Touching : SegmentRelation::Disjoint;
    if (is_degenerate(t)) return on_segment(s, t.a) ? SegmentRelation::Touching : SegmentRelation::Disjoint;

    int s1 = ccw(s.a, s.b, t.a), s2 = ccw(s.a, s.b, t.b);
    int t1 = ccw(t.a, t.b, s.a), t2 = ccw(t.a, t.b, s.b);
    if (s1 * s2 < 0 && t1 * t2 < 0) return SegmentRelation::Proper;

    if (s1 == 0 && s2 == 0) {
        bool use_x = std::abs(s.b.x - s.a.x) >= std::abs(s.b.y - s.a.y);
        auto coordinate = [&](Point p) { return use_x ? p.x : p.y; };
        Real s0 = coordinate(s.a), s3 = coordinate(s.b);
        Real t0 = coordinate(t.a), t3 = coordinate(t.b);
        if (s0 > s3) std::swap(s0, s3);
        if (t0 > t3) std::swap(t0, t3);
        Real overlap = std::min(s3, t3) - std::max(s0, t0);
        if (overlap < -EPS) return SegmentRelation::Disjoint;
        return overlap <= EPS ? SegmentRelation::Touching : SegmentRelation::Overlapping;
    }

    if ((!s1 && on_segment(s, t.a)) || (!s2 && on_segment(s, t.b)) ||
        (!t1 && on_segment(t, s.a)) || (!t2 && on_segment(t, s.b))) {
        return SegmentRelation::Touching;
    }
    return SegmentRelation::Disjoint;
}
inline bool segments_intersect(Segment s, Segment t) {
    return segment_relation(s, t) != SegmentRelation::Disjoint;
}
template<class L, std::enable_if_t<std::is_same_v<std::decay_t<L>, Line>, int> = 0>
inline bool segments_intersect(const L& s, const L& t) {
    return segments_intersect(Segment{s.a, s.b}, Segment{t.a, t.b});
}
inline Real distance_segments(Segment s, Segment t) {
    if (segments_intersect(s, t)) return 0;
    return std::min({distance_segment_point(s, t.a), distance_segment_point(s, t.b),
                     distance_segment_point(t, s.a), distance_segment_point(t, s.b)});
}
template<class L, std::enable_if_t<std::is_same_v<std::decay_t<L>, Line>, int> = 0>
inline Real distance_segments(const L& s, const L& t) {
    return distance_segments(Segment{s.a, s.b}, Segment{t.a, t.b});
}

inline bool intersects(Line a, Line b) { return line_relation(a, b) != LineRelation::Parallel; }
inline bool intersects(Segment a, Segment b) { return segments_intersect(a, b); }
inline bool intersects(Line line, Segment segment) {
    assert(!is_degenerate(line));
    if (is_degenerate(segment)) return on_line(line, segment.a);
    int side_a = ccw(line.a, line.b, segment.a), side_b = ccw(line.a, line.b, segment.b);
    return side_a == 0 || side_b == 0 || side_a != side_b;
}
inline bool intersects(Segment segment, Line line) { return intersects(line, segment); }

inline bool on_circle(Circle circle, Point p) {
    assert(circle.radius >= 0);
    return sign(abs(p - circle.center) - circle.radius) == 0;
}
inline bool inside_circle(Circle circle, Point p) {
    assert(circle.radius >= 0);
    return sign(abs(p - circle.center) - circle.radius) < 0;
}
inline bool inside_or_on_circle(Circle circle, Point p) {
    assert(circle.radius >= 0);
    return sign(abs(p - circle.center) - circle.radius) <= 0;
}

inline CircleRelation circle_relation(Circle a, Circle b) {
    assert(a.radius >= 0 && b.radius >= 0);
    Real distance = abs(b.center - a.center);
    if (distance <= EPS && std::abs(a.radius - b.radius) <= EPS) return CircleRelation::Coincident;
    Real sum = a.radius + b.radius, difference = std::abs(a.radius - b.radius);
    int versus_sum = sign(distance - sum), versus_difference = sign(distance - difference);
    if (versus_sum > 0) return CircleRelation::Separate;
    if (versus_sum == 0) return CircleRelation::ExternallyTangent;
    if (versus_difference < 0) return CircleRelation::Contained;
    if (versus_difference == 0) return CircleRelation::InternallyTangent;
    return CircleRelation::Intersecting;
}

inline bool intersects(Line line, Circle circle) {
    assert(circle.radius >= 0);
    return distance_line_point(line, circle.center) <= circle.radius + EPS;
}
inline bool intersects(Circle circle, Line line) { return intersects(line, circle); }
inline bool intersects(Circle a, Circle b) {
    CircleRelation relation = circle_relation(a, b);
    return relation == CircleRelation::ExternallyTangent || relation == CircleRelation::Intersecting ||
           relation == CircleRelation::InternallyTangent || relation == CircleRelation::Coincident;
}

// Returns nullopt for parallel or coincident lines.
inline std::optional<Point> line_intersection(Line l, Line m) {
    assert(!is_degenerate(l) && !is_degenerate(m));
    Point ld = direction(l), md = direction(m);
    Real denominator = cross(ld, md);
    if (std::abs(denominator) <= EPS) return std::nullopt;
    return l.a + ld * (cross(m.a - l.a, md) / denominator);
}
inline std::vector<Point> circle_line_intersections(Circle c, Line l) {
    assert(c.radius >= 0 && !is_degenerate(l));
    Point foot = projection(l, c.center), unit = direction(l) / abs(direction(l));
    Real h2 = c.radius * c.radius - norm2(foot - c.center);
    if (h2 < -EPS) return {};
    if (h2 <= EPS) return {foot};
    Point offset = unit * std::sqrt(std::max<Real>(0, h2));
    return {foot - offset, foot + offset};
}
inline std::vector<Point> circle_segment_intersections(Circle circle, Segment segment) {
    if (is_degenerate(segment)) return on_circle(circle, segment.a) ? std::vector<Point>{segment.a} : std::vector<Point>{};
    std::vector<Point> result;
    for (Point p : circle_line_intersections(circle, Line{segment.a, segment.b})) {
        if (on_segment(segment, p)) result.push_back(p);
    }
    return result;
}
inline bool intersects(Segment segment, Circle circle) {
    return !circle_segment_intersections(circle, segment).empty();
}
inline bool intersects(Circle circle, Segment segment) { return intersects(segment, circle); }
inline std::vector<Point> circle_intersections(Circle a, Circle b) {
    assert(a.radius >= 0 && b.radius >= 0);
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
