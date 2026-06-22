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
using i128 = __int128_t;

static void test_integer_geometry() {
    using namespace geometry_int;
    assert(ccw(IPoint{0, 0}, IPoint{2, 0}, IPoint{1, 1}) == 1);
    assert(on_segment(IPoint{0, 0}, IPoint{4, 2}, IPoint{2, 1}));
    assert(segments_intersect(IPoint{0, 0}, IPoint{4, 4}, IPoint{0, 4}, IPoint{4, 0}));
    assert(segments_intersect(IPoint{0, 0}, IPoint{2, 0}, IPoint{1, 0}, IPoint{3, 0}));
    assert(!segments_intersect(IPoint{0, 0}, IPoint{1, 0}, IPoint{2, 0}, IPoint{3, 0}));
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
}

static bool close(long double a, long double b) { return abs(a - b) < 1e-10L; }
static void test_real_geometry() {
    namespace gr = geometry_real;
    gr::Line horizontal{{0, 0}, {2, 0}}, vertical{{1, -2}, {1, 2}};
    auto intersection = gr::line_intersection(horizontal, vertical);
    assert(intersection && close(intersection->x, 1) && close(intersection->y, 0));
    auto projected = gr::projection(horizontal, {1, 3});
    assert(close(projected.x, 1) && close(projected.y, 0));
    assert(close(gr::distance_segment_point(horizontal, {3, 4}), sqrtl(17)));
    assert(gr::segments_intersect(horizontal, vertical));
    assert(close(gr::distance_segments({{0, 0}, {1, 0}}, {{2, 1}, {2, 3}}), sqrtl(2)));
    auto line_hits = gr::circle_line_intersections({{0, 0}, 2}, horizontal);
    assert(line_hits.size() == 2 && close(gr::abs(line_hits[0]), 2) && close(gr::abs(line_hits[1]), 2));
    auto tangent = gr::circle_intersections({{0, 0}, 1}, {{2, 0}, 1});
    assert(tangent.size() == 1 && close(tangent[0].x, 1));
    auto two_hits = gr::circle_intersections({{0, 0}, 2}, {{2, 0}, 2});
    assert(two_hits.size() == 2);
    for (auto p : two_hits) assert(close(gr::abs(p), 2) && close(gr::abs(p - gr::Point{2, 0}), 2));
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
    test_integer_geometry(); test_real_geometry(); test_closest_pair(); test_rectangle_area();
    test_kd_tree(); test_interval_set(); test_matching();
    cout << "geometry and more tests passed\n";
}
