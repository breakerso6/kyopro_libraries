#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include <bits/stdc++.h>
using namespace std;

#include "libraries/geometry/GeometryInt.hpp"
#include "libraries/geometry/GeometryReal.hpp"
#include "libraries/geometry/ClosestPair.hpp"
#include "libraries/geometry/RectangleUnionArea.hpp"
#include "libraries/geometry/KDTree.hpp"
#include "libraries/data_structure/IntervalSet.hpp"
#include "libraries/graph/HopcroftKarp.hpp"

using IPoint = geometry_int::Point<long long>;
using ILine = geometry_int::Line<long long>;
using ISegment = geometry_int::Segment<long long>;
using ICircle = geometry_int::Circle<long long>;
using i128 = __int128_t;

static void test_integer_geometry() {
    using namespace geometry_int;
    pair<long long, long long> point_pair{2, 3};
    IPoint point_from_pair = point_pair;
    assert((point_from_pair == IPoint{2, 3}));
    ILine line_from_pairs(pair<long long, long long>{0, 0}, pair<int, int>{2, 2});
    ILine line_from_pair_of_pairs = pair{pair{0LL, 0LL}, pair{3LL, 3LL}};
    ISegment segment_from_pair = pair{pair{1LL, 2LL}, pair{3LL, 4LL}};
    ICircle circle_from_pair = pair{pair{5LL, 6LL}, 7LL};
    assert(line_from_pairs == line_from_pair_of_pairs);
    assert((line_from_pairs != ILine{{0, 1}, {2, 3}}));
    assert((segment_from_pair == ISegment{{3, 4}, {1, 2}}));
    assert((segment_from_pair != ISegment{{1, 2}, {4, 3}}));
    assert((circle_from_pair == ICircle{{5, 6}, 7}));
    assert((circle_from_pair != ICircle{{5, 6}, 8}));

    assert(ccw(IPoint{0, 0}, IPoint{2, 0}, IPoint{1, 1}) == 1);
    assert(on_segment(IPoint{0, 0}, IPoint{4, 2}, IPoint{2, 1}));
    assert(segments_intersect(IPoint{0, 0}, IPoint{4, 4}, IPoint{0, 4}, IPoint{4, 0}));
    assert(segments_intersect(IPoint{0, 0}, IPoint{2, 0}, IPoint{1, 0}, IPoint{3, 0}));
    assert(!segments_intersect(IPoint{0, 0}, IPoint{1, 0}, IPoint{2, 0}, IPoint{3, 0}));

    ILine horizontal{{0, 0}, {4, 0}}, parallel_line{{0, 2}, {8, 2}};
    ILine vertical{{2, -3}, {2, 3}}, same_horizontal{{-2, 0}, {7, 0}};
    assert(is_degenerate(ISegment{{1, 1}, {1, 1}}));
    assert(!is_parallel(ISegment{{1, 1}, {1, 1}}, horizontal));
    assert(is_parallel(horizontal, parallel_line));
    assert(is_perpendicular(horizontal, vertical));
    assert(line_relation(horizontal, vertical) == LineRelation::Intersecting);
    assert(line_relation(horizontal, parallel_line) == LineRelation::Parallel);
    assert(line_relation(horizontal, same_horizontal) == LineRelation::Coincident);
    assert(intersects(horizontal, vertical) && !intersects(horizontal, parallel_line));
    assert(intersects(horizontal, same_horizontal));
    assert(on_line(horizontal, {100, 0}) && !on_line(horizontal, {0, 1}));

    ISegment diagonal_a{{0, 0}, {4, 4}}, diagonal_b{{0, 4}, {4, 0}};
    assert(segment_relation(diagonal_a, diagonal_b) == SegmentRelation::Proper);
    assert(segment_relation(ISegment{{0, 0}, {2, 0}}, ISegment{{2, 0}, {4, 0}}) == SegmentRelation::Touching);
    assert(segment_relation(ISegment{{0, 0}, {3, 0}}, ISegment{{1, 0}, {4, 0}}) == SegmentRelation::Overlapping);
    assert(segment_relation(ISegment{{0, 0}, {1, 0}}, ISegment{{2, 0}, {3, 0}}) == SegmentRelation::Disjoint);
    assert(segment_relation(ISegment{{1, 1}, {1, 1}}, diagonal_a) == SegmentRelation::Touching);
    assert(intersects(vertical, ISegment{{0, 0}, {4, 0}}));
    assert(!intersects(vertical, ISegment{{3, 0}, {4, 0}}));

    ICircle circle{{0, 0}, 5};
    assert(on_circle(circle, {3, 4}));
    assert(inside_circle(circle, {0, 0}));
    assert(inside_or_on_circle(circle, {3, 4}));
    assert(!inside_or_on_circle(circle, {6, 0}));
    assert(circle_relation(circle, {{10, 0}, 5}) == CircleRelation::ExternallyTangent);
    assert(circle_relation(circle, {{6, 0}, 5}) == CircleRelation::Intersecting);
    assert(circle_relation(circle, {{1, 0}, 4}) == CircleRelation::InternallyTangent);
    assert(circle_relation(circle, {{0, 0}, 3}) == CircleRelation::Contained);
    assert(circle_relation(circle, {{11, 0}, 5}) == CircleRelation::Separate);
    assert(circle_relation(circle, circle) == CircleRelation::Coincident);
    assert(intersects(ILine{{-10, 5}, {10, 5}}, circle));
    assert(!intersects(ILine{{-10, 6}, {10, 6}}, circle));
    assert(intersects(ISegment{{-10, 0}, {10, 0}}, circle));
    assert(intersects(ISegment{{5, 0}, {8, 0}}, circle));
    assert(!intersects(ISegment{{-1, 0}, {1, 0}}, circle));
    assert(!intersects(ISegment{{6, 0}, {8, 0}}, circle));
    assert(intersects(circle, ICircle{{10, 0}, 5}));
    assert(!intersects(circle, ICircle{{11, 0}, 5}));

    vector<IPoint> points{{0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 0}, {1, 1}, {0, 0}};
    auto hull = convex_hull(points);
    assert((hull == vector<IPoint>{{0, 0}, {2, 0}, {2, 2}, {0, 2}}));
    assert(polygon_area2(hull) == 8 && convex_diameter2(hull) == 8);
    assert(point_in_polygon(hull, {1, 1}) == 1);
    assert(point_in_polygon(hull, {2, 1}) == 0);
    assert(point_in_polygon(hull, {3, 1}) == -1);
    assert(lattice_points_on_boundary(hull) == 8);
    IPoint large_a{4'000'000'000LL, 0}, large_b{0, 4'000'000'000LL};
    assert(cross(large_a, large_b) == (i128)16'000'000'000'000'000'000ULL);
    mt19937 rng(2026);
    for (int trial = 0; trial < 500; ++trial) {
        vector<IPoint> random_points(3 + rng() % 30);
        for (auto& p : random_points) p = {(int)(rng() % 101) - 50, (int)(rng() % 101) - 50};
        auto random_hull = convex_hull(random_points);
        i128 expected = 0;
        for (auto a : random_hull) for (auto b : random_hull) expected = max(expected, distance2(a, b));
        assert(convex_diameter2(random_hull) == expected);
    }
    for (int trial = 0; trial < 5000; ++trial) {
        auto random_point = [&]() -> IPoint {
            return {(int)(rng() % 21) - 10, (int)(rng() % 21) - 10};
        };
        ISegment a{random_point(), random_point()}, b{random_point(), random_point()};
        bool legacy = segments_intersect(a.a, a.b, b.a, b.b);
        assert(segments_intersect(a, b) == legacy);
        assert((segment_relation(a, b) != SegmentRelation::Disjoint) == legacy);
        assert(segment_relation(a, b) == segment_relation(b, a));
    }
}

static void test_rational_geometry() {
    using namespace geometry_int;
    auto r = [](long long numerator, long long denominator = 1) {
        return Rational(numerator, denominator);
    };

    assert(r(-6, -8) == r(3, 4));
    assert(r(1, 2) + r(1, 3) == r(5, 6));
    assert(r(2, 3) * r(9, 4) == r(3, 2));
    assert(r(2, 3) / r(-4, 5) == r(-5, 6));
    assert(r(-1, 2) < r(1, 3));
    assert(r(-7, 3).floor() == -3 && r(-7, 3).ceil() == -2);
    assert(r(7, 3).floor() == 2 && r(7, 3).ceil() == 3);
    assert(r(4, 2).is_integer() && !r(1, 2).is_integer());
    assert(std::abs(r(1, 8).to_long_double() - 0.125L) < 1e-18L);

    RationalPoint a{r(0), r(0)}, b{r(1), r(0)}, c{r(0), r(1)};
    assert(cross(a, b, c) == r(1));
    assert(ccw(a, b, c) == 1);
    assert(triangle_area2(a, b, c) == r(1));
    assert(segment_relation(RationalSegment{a, b}, RationalSegment{{r(1, 2), r(0)}, {r(2), r(0)}})
           == SegmentRelation::Overlapping);

    auto half = midpoint(IPoint{0, 0}, IPoint{1, 1});
    assert((half == RationalPoint{r(1, 2), r(1, 2)}));
    assert((midpoint(ISegment{{-1, 2}, {2, -1}}) == RationalPoint{r(1, 2), r(1, 2)}));

    ILine first{{0, 0}, {2, 1}}, second{{0, 1}, {1, 0}};
    auto intersection = line_intersection(first, second);
    assert((intersection && *intersection == RationalPoint{r(2, 3), r(1, 3)}));
    assert(!line_intersection(ILine{{0, 0}, {1, 0}}, ILine{{0, 1}, {1, 1}}));
    assert(!line_intersection(ILine{{0, 0}, {1, 0}}, ILine{{2, 0}, {3, 0}}));

    auto projected = projection(ILine{{0, 0}, {1, 1}}, IPoint{1, 0});
    assert((projected == RationalPoint{r(1, 2), r(1, 2)}));
    assert((reflection(ILine{{0, 0}, {1, 1}}, IPoint{1, 0}) == RationalPoint{r(0), r(1)}));

    auto bisector = perpendicular_bisector(IPoint{0, 0}, IPoint{1, 0});
    assert(on_line(bisector, RationalPoint{r(1, 2), r(100)}));
    assert(is_perpendicular(bisector, to_rational(ILine{{0, 0}, {1, 0}})));
    auto through = perpendicular_line(ILine{{0, 0}, {2, 0}}, IPoint{3, 4});
    assert(on_line(through, RationalPoint{r(3), r(-100)}));
    auto parallel = parallel_line(ILine{{0, 0}, {2, 1}}, IPoint{3, 4});
    assert(is_parallel(parallel, to_rational(ILine{{0, 0}, {2, 1}})));

    auto crossing = segment_intersection_points(ISegment{{0, 0}, {2, 1}}, ISegment{{0, 1}, {1, 0}});
    assert((crossing == vector<RationalPoint>{RationalPoint{r(2, 3), r(1, 3)}}));
    auto overlap = segment_intersection_points(ISegment{{0, 0}, {3, 0}}, ISegment{{1, 0}, {4, 0}});
    assert((overlap == vector<RationalPoint>{{r(1), r(0)}, {r(3), r(0)}}));
    assert(segment_intersection_points(ISegment{{0, 0}, {1, 0}}, ISegment{{2, 0}, {3, 0}}).empty());

    assert((triangle_centroid(IPoint{0, 0}, IPoint{1, 0}, IPoint{0, 1})
            == RationalPoint{r(1, 3), r(1, 3)}));
    auto center = circumcenter(IPoint{0, 0}, IPoint{2, 0}, IPoint{0, 2});
    assert((center && *center == RationalPoint{r(1), r(1)}));
    auto altitude = orthocenter(IPoint{0, 0}, IPoint{2, 0}, IPoint{0, 2});
    assert((altitude && *altitude == RationalPoint{r(0), r(0)}));
    assert(!circumcenter(IPoint{0, 0}, IPoint{1, 0}, IPoint{2, 0}));
    assert(!orthocenter(IPoint{0, 0}, IPoint{1, 0}, IPoint{2, 0}));

    vector<RationalPoint> rational_square{{r(0), r(0)}, {r(1, 2), r(0)},
                                           {r(1, 2), r(1, 2)}, {r(0), r(1, 2)}};
    assert(signed_polygon_area2(rational_square) == r(1, 2));
    assert(polygon_area2(rational_square) == r(1, 2));
    auto rational_hull = convex_hull(vector<RationalPoint>{rational_square[0], rational_square[1],
                                                            rational_square[2], rational_square[3],
                                                            RationalPoint{r(1, 4), r(1, 4)}});
    assert(rational_hull == rational_square);
    assert(point_in_polygon(rational_hull, RationalPoint{r(1, 4), r(1, 4)}) == 1);
    assert(convex_diameter2(rational_hull) == r(1, 2));
    RationalCircle small_circle{{r(0), r(0)}, r(1, 2)};
    assert(on_circle(small_circle, RationalPoint{r(1, 2), r(0)}));
    assert(circle_relation(small_circle, RationalCircle{{r(1), r(0)}, r(1, 2)})
           == CircleRelation::ExternallyTangent);

    mt19937 rng(86420);
    for (int trial = 0; trial < 2000; ++trial) {
        auto random_point = [&]() -> IPoint {
            return {(int)(rng() % 21) - 10, (int)(rng() % 21) - 10};
        };
        ILine left{random_point(), random_point()}, right{random_point(), random_point()};
        if (is_degenerate(left) || is_degenerate(right) || is_parallel(left, right)) continue;
        auto point = line_intersection(left, right);
        assert(point);
        assert(on_line(to_rational(left), *point));
        assert(on_line(to_rational(right), *point));
        assert(line_intersection(right, left) == point);
    }
}

static bool close(long double a, long double b) { return abs(a - b) < 1e-10L; }
static void test_real_geometry() {
    namespace gr = geometry_real;
    gr::Point point_from_pair = pair{2.0, 3.0};
    gr::Line line_from_pair = pair{pair{0.0, 0.0}, pair{1.0, 1.0}};
    gr::Segment segment_from_pairs(pair{1.0, 2.0}, pair{3.0, 4.0});
    gr::Circle circle_from_pair = pair{pair{5.0, 6.0}, 7.0};
    assert(point_from_pair == gr::Point(2.0L + gr::EPS / 2, 3.0L));
    assert(line_from_pair == gr::Line({2, 2}, {4, 4}));
    assert(line_from_pair != gr::Line({0, 1}, {1, 2}));
    assert(segment_from_pairs == gr::Segment({3, 4}, {1, 2}));
    assert(segment_from_pairs != gr::Segment({1, 2}, {4, 3}));
    assert(circle_from_pair == gr::Circle({5, 6}, 7 + gr::EPS / 2));
    assert(circle_from_pair != gr::Circle({5, 6}, 8));

    gr::Line horizontal{{0, 0}, {2, 0}}, vertical{{1, -2}, {1, 2}};
    gr::Line parallel_line{{0, 1}, {4, 1}}, same_horizontal{{-3, 0}, {7, 0}};
    assert(gr::is_degenerate(gr::Segment{{1, 1}, {1, 1}}));
    assert(!gr::is_parallel(gr::Segment{{1, 1}, {1, 1}}, horizontal));
    assert(gr::is_parallel(horizontal, parallel_line));
    assert(gr::is_perpendicular(horizontal, vertical));
    assert(gr::line_relation(horizontal, vertical) == gr::LineRelation::Intersecting);
    assert(gr::line_relation(horizontal, parallel_line) == gr::LineRelation::Parallel);
    assert(gr::line_relation(horizontal, same_horizontal) == gr::LineRelation::Coincident);
    assert(gr::intersects(horizontal, same_horizontal));
    auto intersection = gr::line_intersection(horizontal, vertical);
    assert(intersection && close(intersection->x, 1) && close(intersection->y, 0));
    auto projected = gr::projection(horizontal, {1, 3});
    assert(close(projected.x, 1) && close(projected.y, 0));
    assert(close(gr::distance_segment_point(horizontal, {3, 4}), sqrtl(17)));
    assert(gr::segments_intersect(horizontal, vertical));
    assert(close(gr::distance_segments({{0, 0}, {1, 0}}, {{2, 1}, {2, 3}}), sqrtl(2)));

    gr::Segment proper_a{{0, 0}, {4, 4}}, proper_b{{0, 4}, {4, 0}};
    assert(gr::segment_relation(proper_a, proper_b) == gr::SegmentRelation::Proper);
    assert(gr::segment_relation({{0, 0}, {2, 0}}, {{2, 0}, {4, 0}}) == gr::SegmentRelation::Touching);
    assert(gr::segment_relation({{0, 0}, {3, 0}}, {{1, 0}, {4, 0}}) == gr::SegmentRelation::Overlapping);
    assert(gr::segment_relation({{0, 0}, {1, 0}}, {{2, 0}, {3, 0}}) == gr::SegmentRelation::Disjoint);
    assert(gr::intersects(vertical, gr::Segment{{0, 0}, {2, 0}}));

    auto line_hits = gr::circle_line_intersections({{0, 0}, 2}, horizontal);
    assert(line_hits.size() == 2 && close(gr::abs(line_hits[0]), 2) && close(gr::abs(line_hits[1]), 2));
    auto tangent = gr::circle_intersections({{0, 0}, 1}, {{2, 0}, 1});
    assert(tangent.size() == 1 && close(tangent[0].x, 1));
    auto two_hits = gr::circle_intersections({{0, 0}, 2}, {{2, 0}, 2});
    assert(two_hits.size() == 2);
    for (auto p : two_hits) assert(close(gr::abs(p), 2) && close(gr::abs(p - gr::Point{2, 0}), 2));

    gr::Circle circle{{0, 0}, 5};
    assert(gr::on_circle(circle, {3, 4}) && gr::inside_circle(circle, {0, 0}));
    assert(gr::circle_relation(circle, {{10, 0}, 5}) == gr::CircleRelation::ExternallyTangent);
    assert(gr::circle_relation(circle, {{6, 0}, 5}) == gr::CircleRelation::Intersecting);
    assert(gr::circle_relation(circle, {{1, 0}, 4}) == gr::CircleRelation::InternallyTangent);
    assert(gr::circle_relation(circle, {{0, 0}, 3}) == gr::CircleRelation::Contained);
    assert(gr::circle_relation(circle, circle) == gr::CircleRelation::Coincident);
    assert(gr::intersects(gr::Line{{-10, 5}, {10, 5}}, circle));
    assert(!gr::intersects(gr::Line{{-10, 6}, {10, 6}}, circle));
    auto segment_hits = gr::circle_segment_intersections(circle, {{-10, 0}, {10, 0}});
    assert(segment_hits.size() == 2);
    assert(gr::intersects(gr::Segment{{5, 0}, {8, 0}}, circle));
    assert(!gr::intersects(gr::Segment{{-1, 0}, {1, 0}}, circle));
}

static void test_closest_pair() {
    mt19937_64 rng(12345);
    assert(geometry_int::closest_pair_distance2(vector<IPoint>{{0, 0}}) == -1);
    for (int n = 2; n <= 50; ++n) for (int trial = 0; trial < 30; ++trial) {
        vector<IPoint> points(n);
        for (auto& p : points) p = {(long long)(rng() % 101) - 50, (long long)(rng() % 101) - 50};
        i128 expected = (i128(1) << 126);
        for (int i = 0; i < n; ++i) for (int j = i + 1; j < n; ++j)
            expected = min(expected, geometry_int::distance2(points[i], points[j]));
        assert(geometry_int::closest_pair_distance2(points) == expected);
    }
}

static void test_rectangle_area() {
    vector<array<long long, 4>> example{{0, 0, 3, 2}, {1, 1, 4, 4}, {4, 0, 4, 8}};
    assert(rectangle_union_area(example) == 13);
    mt19937 rng(777);
    for (int trial = 0; trial < 500; ++trial) {
        vector<array<long long, 4>> rectangles;
        bool covered[10][10]{};
        int n = rng() % 10;
        for (int i = 0; i < n; ++i) {
            int x1 = rng() % 10, x2 = rng() % 10, y1 = rng() % 10, y2 = rng() % 10;
            if (x1 > x2) swap(x1, x2);
            if (y1 > y2) swap(y1, y2);
            rectangles.push_back({x1, y1, x2, y2});
            for (int x = x1; x < x2; ++x) for (int y = y1; y < y2; ++y) covered[x][y] = true;
        }
        int expected = 0; for (auto& row : covered) for (bool cell : row) expected += cell;
        assert(rectangle_union_area(rectangles) == expected);
    }
}

static void test_kd_tree() {
    mt19937 rng(4567);
    vector<IPoint> points(200);
    for (auto& p : points) p = {(int)(rng() % 201) - 100, (int)(rng() % 201) - 100};
    KDTree tree(points);
    for (int trial = 0; trial < 1000; ++trial) {
        IPoint query{(int)(rng() % 301) - 150, (int)(rng() % 301) - 150};
        pair<i128, int> expected{(i128(1) << 126), -1};
        for (int i = 0; i < (int)points.size(); ++i) expected = min(expected, {geometry_int::distance2(points[i], query), i});
        assert(tree.nearest(query) == expected);
        long long x1 = (int)(rng() % 201) - 100, x2 = (int)(rng() % 201) - 100;
        long long y1 = (int)(rng() % 201) - 100, y2 = (int)(rng() % 201) - 100;
        if (x1 > x2) swap(x1, x2);
        if (y1 > y2) swap(y1, y2);
        auto got = tree.range_search(x1, y1, x2, y2); sort(got.begin(), got.end());
        vector<int> want;
        for (int i = 0; i < (int)points.size(); ++i)
            if (x1 <= points[i].x && points[i].x < x2 && y1 <= points[i].y && points[i].y < y2) want.push_back(i);
        assert(got == want);
    }
    assert((KDTree<long long>({}).nearest({0, 0}) == pair<i128, int>{-1, -1}));
}

static void test_interval_set() {
    mt19937 rng(999); IntervalSet<int> set; vector<bool> covered(100);
    for (int trial = 0; trial < 10000; ++trial) {
        int l = rng() % 101, r = rng() % 101; if (l > r) swap(l, r);
        if (rng() & 1) { set.insert(l, r); for (int x = l; x < r; ++x) covered[x] = true; }
        else { set.erase(l, r); for (int x = l; x < r; ++x) covered[x] = false; }
        for (int x = 0; x < 100; ++x) assert(set.contains(x) == covered[x]);
        for (int x = 0; x < 100; ++x) {
            int expected = x; while (expected < 100 && covered[expected]) ++expected;
            assert(set.mex(x) == expected);
        }
    }
}

static int brute_matching(const vector<vector<int>>& graph, int v, int used) {
    if (v == (int)graph.size()) return 0;
    int answer = brute_matching(graph, v + 1, used);
    for (int to : graph[v]) if (!(used >> to & 1)) answer = max(answer, 1 + brute_matching(graph, v + 1, used | 1 << to));
    return answer;
}
static void test_matching() {
    mt19937 rng(31415);
    for (int trial = 0; trial < 1000; ++trial) {
        int n = rng() % 7, m = rng() % 7; HopcroftKarp matching(n, m);
        for (int l = 0; l < n; ++l) for (int r = 0; r < m; ++r) if (rng() % 3 == 0) matching.add_edge(l, r);
        int expected = brute_matching(matching.graph, 0, 0);
        assert(matching.max_matching() == expected);
        assert(matching.max_matching() == expected);
        auto [left, right] = matching.minimum_vertex_cover();
        assert((int)left.size() + (int)right.size() == expected);
        vector<char> in_left(n), in_right(m);
        for (int v : left) in_left[v] = 1;
        for (int v : right) in_right[v] = 1;
        for (int l = 0; l < n; ++l) for (int r : matching.graph[l]) assert(in_left[l] || in_right[r]);
    }
}

int main() {
    test_integer_geometry(); test_rational_geometry(); test_real_geometry(); test_closest_pair(); test_rectangle_area();
    test_kd_tree(); test_interval_set(); test_matching();
    cout << "Hello World\n";
}
