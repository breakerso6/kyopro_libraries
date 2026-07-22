# GeometryReal

`libraries/geometry/GeometryReal.hpp` は `long double` 座標の2次元幾何を `geometry_real` 名前空間で提供します。判定には `EPS = 1e-12` を使います。

## Include

```cpp
#include "libraries/geometry/GeometryReal.hpp"
```

## 型

```cpp
using Real = long double;

struct Point   { Real x, y; };
struct Line    { Point a, b; };
struct Segment { Point a, b; };
struct Circle  { Point center; Real radius; };
```

- `Line`: 異なる2点を通る無限直線
- `Segment`: 両端点を含む閉線分
- `Circle`: 円板ではなく円周

`Circle::radius` は非負である必要があります。

## コンストラクタ・等値比較

各型は従来の `{...}` に加え、座標や図形を表す `pair` から構築できます。

```cpp
Point();
Point(Real x, Real y);
Point(pair<X, Y> point);

Line(Point a, Point b);
Line(pair<X, Y> a, pair<U, V> b);
Line(pair<A, B> points);

Segment(Point a, Point b);
Segment(pair<X, Y> a, pair<U, V> b);
Segment(pair<A, B> points);

Circle(Point center, Real radius);
Circle(pair<X, Y> center, R radius);
Circle(pair<Center, R> circle);
```

たとえば次の構築ができます。

```cpp
Point p = pair{1.0L, 2.0L};
Line line = pair{pair{0.0L, 0.0L}, pair{2.0L, 2.0L}};
Segment segment(pair{0.0L, 0.0L}, pair{3.0L, 1.0L});
Circle circle = pair{pair{0.0L, 0.0L}, 5.0L};
```

`Point`, `Line`, `Segment`, `Circle` は `==`, `!=` に対応し、`EPS` を使って図形として比較します。

- `Line`: 表現に使った2点が異なっても、同じ無限直線なら等しい
- `Segment`: 端点の順序を無視する
- `Circle`: 中心と半径が等しければ等しい

退化した `Line` 同士は、表す点が同じ場合に限り等しいとします。

## 点・ベクトル・向き

```cpp
Real dot(Point a, Point b);
Real cross(Point a, Point b);
Real norm2(Point p);
Real abs(Point p);
int sign(Real x);
int ccw(Point a, Point b, Point c);
Point rotate(Point p, Real angle);

Point direction(Line line);
Point direction(Segment segment);
bool is_degenerate(Line line);
bool is_degenerate(Segment segment);
bool is_parallel(A a, B b);
bool is_perpendicular(A a, B b);
```

`A`, `B` には `Line` または `Segment` を指定できます。退化した入力の平行・垂直判定は `false` です。

## 直線・線分の判定

```cpp
enum class LineRelation { Intersecting, Parallel, Coincident };
enum class SegmentRelation { Disjoint, Touching, Proper, Overlapping };

bool on_line(Line line, Point p);
bool on_segment(Segment segment, Point p);
LineRelation line_relation(Line a, Line b);
SegmentRelation segment_relation(Segment a, Segment b);

bool intersects(Line a, Line b);
bool intersects(Line line, Segment segment);
bool intersects(Segment a, Segment b);
bool segments_intersect(Segment a, Segment b);
```

`SegmentRelation` の意味は次のとおりです。

| relation | meaning |
| --- | --- |
| `Disjoint` | 共通点なし |
| `Touching` | 1点で接触 |
| `Proper` | 両線分の内部で横切る |
| `Overlapping` | 同一直線上で長さを持って重なる |

退化線分は点として扱います。退化直線は対象外です。

## 射影・距離・交点

```cpp
Point projection(Line line, Point p);
Point reflection(Line line, Point p);
Real distance_line_point(Line line, Point p);
Real distance_segment_point(Segment segment, Point p);
Real distance_segments(Segment a, Segment b);
optional<Point> line_intersection(Line a, Line b);
```

`line_intersection` は平行または一致する直線に `nullopt` を返します。

以前の `Line` が線分も兼ねていたコードとの互換性のため、`distance_segment_point(Line, Point)`、`segments_intersect(Line, Line)`、`distance_segments(Line, Line)` も利用できます。新しいコードでは `Segment` を推奨します。

## 円周

```cpp
enum class CircleRelation {
    Separate,
    ExternallyTangent,
    Intersecting,
    InternallyTangent,
    Contained,
    Coincident
};

bool on_circle(Circle circle, Point p);
bool inside_circle(Circle circle, Point p);
bool inside_or_on_circle(Circle circle, Point p);
CircleRelation circle_relation(Circle a, Circle b);

bool intersects(Line line, Circle circle);
bool intersects(Segment segment, Circle circle);
bool intersects(Circle a, Circle b);

vector<Point> circle_line_intersections(Circle circle, Line line);
vector<Point> circle_segment_intersections(Circle circle, Segment segment);
vector<Point> circle_intersections(Circle a, Circle b);
```

`intersects` は円板ではなく円周同士の共通点を判定します。円の内部に完全に入った線分や円周には `false` を返します。一致する円周は共通点が無限個なので `true` です。

交点関数は有限個の交点を0個、1個、2個の配列で返します。一致する円周など交点が無限個の場合は空配列を返します。

## 使用例

```cpp
using namespace geometry_real;

Line x_axis{{0, 0}, {1, 0}};
Segment vertical{{2, -1}, {2, 1}};
Circle circle{{0, 0}, 2};

auto p = line_intersection(x_axis, {{2, -1}, {2, 1}});
auto hits = circle_segment_intersections(circle, vertical);
bool tangent = circle_relation(circle, {{4, 0}, 2})
             == CircleRelation::ExternallyTangent;
```

## 注意

- 判定には絶対誤差 `EPS` を使います。座標のスケールが極端に大きい問題では、問題に合わせて `EPS` を調整してください。
- `Line` を受け取る関数には異なる2点を指定してください。
- 一致する直線や円周など、交点が無限個になる場合、交点取得関数は `nullopt` または空配列を返します。
