#pragma once
#include <bits/stdc++.h>
#include "libraries/math/Rational.hpp"

namespace geometry_int {
using i128 = __int128_t;

inline i128 abs128(i128 value) { return value < 0 ? -value : value; }
inline i128 gcd128(i128 a, i128 b) {
    a = abs128(a);
    b = abs128(b);
    while (b) {
        i128 remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

using ::Rational;
using Rational128 = Rational<i128>;
using ::abs;
using ::sign;

template<class T>
struct ExactType {
    using type = i128;
    static i128 cast(T value) { return (i128)value; }
};
template<>
struct ExactType<Rational128> {
    using type = Rational128;
    static Rational128 cast(Rational128 value) { return value; }
};
template<class T>
using exact_t = typename ExactType<T>::type;
template<class T>
exact_t<T> exact_cast(T value) { return ExactType<T>::cast(value); }

template<class T = long long>
struct Point {
    T x{}, y{};
    Point operator+(const Point& p) const { return {x + p.x, y + p.y}; }
    Point operator-(const Point& p) const { return {x - p.x, y - p.y}; }
    Point operator*(const T& scale) const { return {x * scale, y * scale}; }
    bool operator==(const Point& p) const { return x == p.x && y == p.y; }
    bool operator!=(const Point& p) const { return !(*this == p); }
    bool operator<(const Point& p) const { return x != p.x ? x < p.x : y < p.y; }
};

template<class T = long long>
struct Line {
    Point<T> a, b;
};

template<class T = long long>
struct Segment {
    Point<T> a, b;
};

// Represents the circumference. radius must be nonnegative.
template<class T = long long>
struct Circle {
    Point<T> center;
    T radius{};
};

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

using RationalPoint = Point<Rational128>;
using RationalLine = Line<Rational128>;
using RationalSegment = Segment<Rational128>;
using RationalCircle = Circle<Rational128>;

template<class T> exact_t<T> dot(Point<T> a, Point<T> b) {
    return exact_cast(a.x) * exact_cast(b.x) + exact_cast(a.y) * exact_cast(b.y);
}
template<class T> exact_t<T> cross(Point<T> a, Point<T> b) {
    return exact_cast(a.x) * exact_cast(b.y) - exact_cast(a.y) * exact_cast(b.x);
}
template<class T> exact_t<T> cross(Point<T> a, Point<T> b, Point<T> c) {
    return cross(b - a, c - a);
}
template<class T> exact_t<T> norm2(Point<T> p) { return dot(p, p); }
template<class T> exact_t<T> distance2(Point<T> a, Point<T> b) {
    exact_t<T> dx = exact_cast(a.x) - exact_cast(b.x);
    exact_t<T> dy = exact_cast(a.y) - exact_cast(b.y);
    return dx * dx + dy * dy;
}
template<class T> exact_t<T> cross_vectors(Point<T> a, Point<T> b, Point<T> c, Point<T> d) {
    return cross(b - a, d - c);
}

template<class T> Point<T> direction(Line<T> line) { return line.b - line.a; }
template<class T> Point<T> direction(Segment<T> segment) { return segment.b - segment.a; }
template<class T> bool is_degenerate(Line<T> line) { return line.a == line.b; }
template<class T> bool is_degenerate(Segment<T> segment) { return segment.a == segment.b; }

template<class T>
bool directions_parallel(Point<T> u, Point<T> v) {
    return u != Point<T>{} && v != Point<T>{} && cross(u, v) == 0;
}
template<class T>
bool directions_perpendicular(Point<T> u, Point<T> v) {
    return u != Point<T>{} && v != Point<T>{} && dot(u, v) == 0;
}
template<class T> bool is_parallel(Line<T> a, Line<T> b) { return directions_parallel(direction(a), direction(b)); }
template<class T> bool is_parallel(Line<T> a, Segment<T> b) { return directions_parallel(direction(a), direction(b)); }
template<class T> bool is_parallel(Segment<T> a, Line<T> b) { return is_parallel(b, a); }
template<class T> bool is_parallel(Segment<T> a, Segment<T> b) { return directions_parallel(direction(a), direction(b)); }
template<class T> bool is_perpendicular(Line<T> a, Line<T> b) { return directions_perpendicular(direction(a), direction(b)); }
template<class T> bool is_perpendicular(Line<T> a, Segment<T> b) { return directions_perpendicular(direction(a), direction(b)); }
template<class T> bool is_perpendicular(Segment<T> a, Line<T> b) { return is_perpendicular(b, a); }
template<class T> bool is_perpendicular(Segment<T> a, Segment<T> b) { return directions_perpendicular(direction(a), direction(b)); }

template<class T>
int ccw(Point<T> a, Point<T> b, Point<T> c) {
    auto value = cross(a, b, c);
    return (value > exact_t<T>{0}) - (value < exact_t<T>{0});
}
template<class T>
bool on_segment(Point<T> a, Point<T> b, Point<T> p) {
    return cross(a, b, p) == exact_t<T>{0} && dot(p - a, p - b) <= exact_t<T>{0};
}
template<class T>
bool on_line(Line<T> line, Point<T> p) {
    assert(!is_degenerate(line));
    return cross(line.a, line.b, p) == 0;
}
template<class T>
bool on_segment(Segment<T> segment, Point<T> p) {
    return on_segment(segment.a, segment.b, p);
}

template<class T>
LineRelation line_relation(Line<T> a, Line<T> b) {
    assert(!is_degenerate(a) && !is_degenerate(b));
    if (!is_parallel(a, b)) return LineRelation::Intersecting;
    return on_line(a, b.a) ? LineRelation::Coincident : LineRelation::Parallel;
}

template<class T>
SegmentRelation segment_relation(Segment<T> a, Segment<T> b) {
    if (is_degenerate(a) && is_degenerate(b)) {
        return a.a == b.a ? SegmentRelation::Touching : SegmentRelation::Disjoint;
    }
    if (is_degenerate(a)) return on_segment(b, a.a) ? SegmentRelation::Touching : SegmentRelation::Disjoint;
    if (is_degenerate(b)) return on_segment(a, b.a) ? SegmentRelation::Touching : SegmentRelation::Disjoint;

    int ab_c = ccw(a.a, a.b, b.a), ab_d = ccw(a.a, a.b, b.b);
    int cd_a = ccw(b.a, b.b, a.a), cd_b = ccw(b.a, b.b, a.b);
    if (ab_c * ab_d < 0 && cd_a * cd_b < 0) return SegmentRelation::Proper;

    if (ab_c == 0 && ab_d == 0) {
        Point<T> a0 = std::min(a.a, a.b), a1 = std::max(a.a, a.b);
        Point<T> b0 = std::min(b.a, b.b), b1 = std::max(b.a, b.b);
        Point<T> left = std::max(a0, b0), right = std::min(a1, b1);
        if (right < left) return SegmentRelation::Disjoint;
        return left == right ? SegmentRelation::Touching : SegmentRelation::Overlapping;
    }

    if ((ab_c == 0 && on_segment(a, b.a)) || (ab_d == 0 && on_segment(a, b.b)) ||
        (cd_a == 0 && on_segment(b, a.a)) || (cd_b == 0 && on_segment(b, a.b))) {
        return SegmentRelation::Touching;
    }
    return SegmentRelation::Disjoint;
}

template<class T>
bool segments_intersect(Point<T> a, Point<T> b, Point<T> c, Point<T> d) {
    return segment_relation(Segment<T>{a, b}, Segment<T>{c, d}) != SegmentRelation::Disjoint;
}
template<class T>
bool segments_intersect(Segment<T> a, Segment<T> b) {
    return segment_relation(a, b) != SegmentRelation::Disjoint;
}

template<class T>
bool intersects(Line<T> a, Line<T> b) {
    return line_relation(a, b) != LineRelation::Parallel;
}
template<class T>
bool intersects(Segment<T> a, Segment<T> b) {
    return segments_intersect(a, b);
}
template<class T>
bool intersects(Line<T> line, Segment<T> segment) {
    assert(!is_degenerate(line));
    if (is_degenerate(segment)) return on_line(line, segment.a);
    int side_a = ccw(line.a, line.b, segment.a);
    int side_b = ccw(line.a, line.b, segment.b);
    return side_a == 0 || side_b == 0 || side_a != side_b;
}
template<class T>
bool intersects(Segment<T> segment, Line<T> line) {
    return intersects(line, segment);
}

template<class T>
Rational128 to_rational(T value) {
    if constexpr (std::is_same_v<std::decay_t<T>, Rational128>) {
        return value;
    } else {
        return Rational128((i128)value);
    }
}
template<class T>
RationalPoint to_rational(Point<T> point) {
    return {to_rational(point.x), to_rational(point.y)};
}
template<class T>
RationalLine to_rational(Line<T> line) {
    return {to_rational(line.a), to_rational(line.b)};
}
template<class T>
RationalSegment to_rational(Segment<T> segment) {
    return {to_rational(segment.a), to_rational(segment.b)};
}
template<class T>
RationalCircle to_rational(Circle<T> circle) {
    return {to_rational(circle.center), to_rational(circle.radius)};
}

template<class T>
RationalPoint midpoint(Point<T> a, Point<T> b) {
    return {(to_rational(a.x) + to_rational(b.x)) / Rational128(2),
            (to_rational(a.y) + to_rational(b.y)) / Rational128(2)};
}
template<class T>
RationalPoint midpoint(Segment<T> segment) {
    return midpoint(segment.a, segment.b);
}

template<class T, class U>
std::optional<RationalPoint> line_intersection(Line<T> a, Line<U> b) {
    RationalLine left = to_rational(a), right = to_rational(b);
    assert(!is_degenerate(left) && !is_degenerate(right));
    RationalPoint left_direction = direction(left), right_direction = direction(right);
    Rational128 denominator = cross(left_direction, right_direction);
    if (denominator == 0) return std::nullopt;
    Rational128 ratio = cross(right.a - left.a, right_direction) / denominator;
    return left.a + left_direction * ratio;
}

template<class T, class U>
RationalPoint projection(Line<T> line, Point<U> point) {
    RationalLine rational_line = to_rational(line);
    RationalPoint rational_point = to_rational(point);
    assert(!is_degenerate(rational_line));
    RationalPoint d = direction(rational_line);
    Rational128 ratio = dot(rational_point - rational_line.a, d) / norm2(d);
    return rational_line.a + d * ratio;
}

template<class T, class U>
RationalPoint reflection(Line<T> line, Point<U> point) {
    RationalPoint rational_point = to_rational(point);
    RationalPoint foot = projection(line, point);
    return foot * Rational128(2) - rational_point;
}

template<class T, class U>
RationalLine parallel_line(Line<T> line, Point<U> point) {
    RationalLine rational_line = to_rational(line);
    RationalPoint through = to_rational(point);
    assert(!is_degenerate(rational_line));
    return {through, through + direction(rational_line)};
}

template<class T, class U>
RationalLine perpendicular_line(Line<T> line, Point<U> point) {
    RationalLine rational_line = to_rational(line);
    RationalPoint through = to_rational(point);
    assert(!is_degenerate(rational_line));
    RationalPoint d = direction(rational_line);
    RationalPoint perpendicular{-d.y, d.x};
    return {through, through + perpendicular};
}

template<class T>
RationalLine perpendicular_bisector(Point<T> a, Point<T> b) {
    assert(a != b);
    RationalPoint center = midpoint(a, b);
    RationalPoint d = to_rational(b) - to_rational(a);
    RationalPoint perpendicular{-d.y, d.x};
    return {center, center + perpendicular};
}
template<class T>
RationalLine perpendicular_bisector(Segment<T> segment) {
    return perpendicular_bisector(segment.a, segment.b);
}

template<class T>
std::vector<RationalPoint> segment_intersection_points(Segment<T> a, Segment<T> b) {
    RationalSegment left = to_rational(a), right = to_rational(b);
    if (!segments_intersect(left, right)) return {};

    std::vector<RationalPoint> result;
    auto add_if_common = [&](RationalPoint point) {
        if (on_segment(left, point) && on_segment(right, point)) result.push_back(point);
    };
    add_if_common(left.a);
    add_if_common(left.b);
    add_if_common(right.a);
    add_if_common(right.b);
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    if (!result.empty()) return result;

    auto point = line_intersection(RationalLine{left.a, left.b}, RationalLine{right.a, right.b});
    assert(point.has_value());
    return {*point};
}

template<class T>
exact_t<T> triangle_area2(Point<T> a, Point<T> b, Point<T> c) {
    exact_t<T> result = cross(a, b, c);
    return result < exact_t<T>{0} ? -result : result;
}

template<class T>
RationalPoint triangle_centroid(Point<T> a, Point<T> b, Point<T> c) {
    return {(to_rational(a.x) + to_rational(b.x) + to_rational(c.x)) / Rational128(3),
            (to_rational(a.y) + to_rational(b.y) + to_rational(c.y)) / Rational128(3)};
}

template<class T>
std::optional<RationalPoint> circumcenter(Point<T> a, Point<T> b, Point<T> c) {
    if (ccw(a, b, c) == 0) return std::nullopt;
    return line_intersection(perpendicular_bisector(a, b), perpendicular_bisector(a, c));
}

template<class T>
std::optional<RationalPoint> orthocenter(Point<T> a, Point<T> b, Point<T> c) {
    if (ccw(a, b, c) == 0) return std::nullopt;
    RationalLine bc{to_rational(b), to_rational(c)};
    RationalLine ac{to_rational(a), to_rational(c)};
    return line_intersection(perpendicular_line(bc, a), perpendicular_line(ac, b));
}

namespace detail {
template<class T>
exact_t<T> wide_distance2(Point<T> a, Point<T> b) {
    exact_t<T> dx = exact_cast(a.x) - exact_cast(b.x);
    exact_t<T> dy = exact_cast(a.y) - exact_cast(b.y);
    return dx * dx + dy * dy;
}
template<class T>
exact_t<T> wide_norm2(Point<T> p) {
    exact_t<T> x = exact_cast(p.x), y = exact_cast(p.y);
    return x * x + y * y;
}
template<class T>
exact_t<T> wide_dot(Point<T> a, Point<T> b) {
    return dot(a, b);
}
template<class T>
exact_t<T> wide_cross(Point<T> a, Point<T> b) {
    return cross(a, b);
}
template<class T>
exact_t<T> radius2(Circle<T> circle) {
    assert(circle.radius >= 0);
    exact_t<T> radius = exact_cast(circle.radius);
    return radius * radius;
}
}  // namespace detail

template<class T>
bool on_circle(Circle<T> circle, Point<T> p) {
    return detail::wide_distance2(circle.center, p) == detail::radius2(circle);
}
template<class T>
bool inside_circle(Circle<T> circle, Point<T> p) {
    return detail::wide_distance2(circle.center, p) < detail::radius2(circle);
}
template<class T>
bool inside_or_on_circle(Circle<T> circle, Point<T> p) {
    return detail::wide_distance2(circle.center, p) <= detail::radius2(circle);
}

template<class T>
CircleRelation circle_relation(Circle<T> a, Circle<T> b) {
    assert(a.radius >= 0 && b.radius >= 0);
    exact_t<T> distance = detail::wide_distance2(a.center, b.center);
    exact_t<T> sum = exact_cast(a.radius) + exact_cast(b.radius);
    exact_t<T> difference = exact_cast(a.radius) - exact_cast(b.radius);
    if (difference < 0) difference = -difference;
    if (distance == 0 && a.radius == b.radius) return CircleRelation::Coincident;
    if (distance > sum * sum) return CircleRelation::Separate;
    if (distance == sum * sum) return CircleRelation::ExternallyTangent;
    if (distance < difference * difference) return CircleRelation::Contained;
    if (distance == difference * difference) return CircleRelation::InternallyTangent;
    return CircleRelation::Intersecting;
}

template<class T>
bool intersects(Line<T> line, Circle<T> circle) {
    assert(!is_degenerate(line));
    Point<T> d = direction(line), offset = circle.center - line.a;
    exact_t<T> area = detail::wide_cross(d, offset);
    return area * area <= detail::radius2(circle) * detail::wide_norm2(d);
}
template<class T> bool intersects(Circle<T> circle, Line<T> line) { return intersects(line, circle); }

template<class T>
bool intersects(Segment<T> segment, Circle<T> circle) {
    exact_t<T> radius2 = detail::radius2(circle);
    exact_t<T> from_a = detail::wide_distance2(segment.a, circle.center) - radius2;
    exact_t<T> from_b = detail::wide_distance2(segment.b, circle.center) - radius2;
    if (from_a == 0 || from_b == 0) return true;
    if ((from_a < 0) != (from_b < 0)) return true;
    if (from_a < 0 && from_b < 0) return false;
    Point<T> d = direction(segment), offset = circle.center - segment.a;
    exact_t<T> length2 = detail::wide_norm2(d);
    if (length2 == 0) return false;
    exact_t<T> projection = detail::wide_dot(offset, d);
    if (projection <= 0 || projection >= length2) return false;
    exact_t<T> area = detail::wide_cross(d, offset);
    return area * area <= radius2 * length2;
}
template<class T> bool intersects(Circle<T> circle, Segment<T> segment) { return intersects(segment, circle); }

template<class T>
bool intersects(Circle<T> a, Circle<T> b) {
    CircleRelation relation = circle_relation(a, b);
    return relation == CircleRelation::ExternallyTangent || relation == CircleRelation::Intersecting ||
           relation == CircleRelation::InternallyTangent || relation == CircleRelation::Coincident;
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
exact_t<T> signed_polygon_area2(const std::vector<Point<T>>& polygon) {
    exact_t<T> result = 0;
    for (int i = 0, n = (int)polygon.size(); i < n; ++i) result += cross(polygon[i], polygon[(i + 1) % n]);
    return result;
}

template<class T>
exact_t<T> polygon_area2(const std::vector<Point<T>>& polygon) {
    exact_t<T> result = signed_polygon_area2(polygon);
    return result < exact_t<T>{0} ? -result : result;
}

// -1: outside, 0: boundary, 1: inside. Polygon may be clockwise.
template<class T>
int point_in_polygon(const std::vector<Point<T>>& polygon, Point<T> p) {
    bool inside = false;
    for (int i = 0, n = (int)polygon.size(); i < n; ++i) {
        Point<T> a = polygon[i], b = polygon[(i + 1) % n];
        if (on_segment(a, b, p)) return 0;
        if ((a.y > p.y) != (b.y > p.y)) {
            auto value = cross(b - a, p - a);
            if ((value > exact_t<T>{0}) == (b.y > a.y)) inside = !inside;
        }
    }
    return inside ? 1 : -1;
}

template<class T>
exact_t<T> convex_diameter2(const std::vector<Point<T>>& polygon) {
    int n = (int)polygon.size();
    if (n <= 1) return 0;
    if (n == 2) return distance2(polygon[0], polygon[1]);
    exact_t<T> answer = 0; int j = 1;
    for (int i = 0; i < n; ++i) {
        int ni = (i + 1) % n;
        while (cross_vectors(polygon[i], polygon[ni], polygon[j], polygon[(j + 1) % n]) > 0) j = (j + 1) % n;
        answer = std::max(answer, distance2(polygon[i], polygon[j]));
        answer = std::max(answer, distance2(polygon[ni], polygon[j]));
    }
    return answer;
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
