# GeometryInt

`libraries/geometry/GeometryInt.hpp` は整数・有理数座標の正確な2次元幾何を `geometry_int` 名前空間で提供します。整数座標の外積や距離は `__int128_t`、有理数座標では `Rational` を返します。

## Include

```cpp
#include "libraries/geometry/GeometryInt.hpp"
```

## 型

```cpp
using ::Rational;
using Rational128 = Rational<__int128_t>;
template<class T = long long> struct Point   { T x, y; };
template<class T = long long> struct Line    { Point<T> a, b; };
template<class T = long long> struct Segment { Point<T> a, b; };
template<class T = long long> struct Circle  { Point<T> center; T radius; };
```

- `Line`: 異なる2点 `a`, `b` を通る無限直線
- `Segment`: 両端点を含む閉線分 `[a,b]`
- `Circle`: 中心 `center`、半径 `radius` の円周。円板ではない

`Point` は加算、減算、等値比較、辞書順比較を持ちます。`Circle::radius` は非負である必要があります。

## コンストラクタ・等値比較

各型は従来の `{...}` に加え、座標や図形を表す `pair` から構築できます。

```cpp
Point<T>();
Point<T>(T x, T y);
Point<T>(pair<X, Y> point);

Line<T>(Point<T> a, Point<T> b);
Line<T>(pair<X, Y> a, pair<U, V> b);
Line<T>(pair<A, B> points);

Segment<T>(Point<T> a, Point<T> b);
Segment<T>(pair<X, Y> a, pair<U, V> b);
Segment<T>(pair<A, B> points);

Circle<T>(Point<T> center, T radius);
Circle<T>(pair<X, Y> center, R radius);
Circle<T>(pair<Center, R> circle);
```

たとえば次の構築ができます。

```cpp
Point<long long> p = pair{1LL, 2LL};
Line<long long> line = pair{pair{0LL, 0LL}, pair{2LL, 2LL}};
Segment<long long> segment(pair{0LL, 0LL}, pair{3LL, 1LL});
Circle<long long> circle = pair{pair{0LL, 0LL}, 5LL};
```

`Point`, `Line`, `Segment`, `Circle` は `==`, `!=` に対応します。比較は図形として行われます。

- `Line`: 表現に使った2点が異なっても、同じ無限直線なら等しい
- `Segment`: 端点の順序を無視する
- `Circle`: 中心と半径が等しければ等しい

退化した `Line` 同士は、表す点が同じ場合に限り等しいとします。

## Rational

```cpp
using ::Rational;
using Rational128 = Rational<__int128_t>;
using RationalPoint = Point<Rational128>;
using RationalLine = Line<Rational128>;
using RationalSegment = Segment<Rational128>;
using RationalCircle = Circle<Rational128>;
```

`Rational` は [math/Rational.hpp](../math/Rational.md) の共通クラステンプレートを再公開したものです。幾何の正確な構成結果には `Rational128 = Rational<__int128_t>` を使います。常に既約で、分母が正になるよう正規化されます。四則演算、単項符号、四則代入、大小・等値比較に対応します。

```cpp
Rational128 a(1, 2), b(1, 3);
Rational128 c = a + b; // 5/6
```

分母に `0` は指定できません。演算・比較途中の積を含め、分子と分母は `__int128_t` に収まる必要があります。

`Point<Rational128>` は整数版と同じ `dot`, `cross`, `ccw`, `on_line`, `on_segment`, `line_relation`, `segment_relation`, `intersects` などで利用できます。

## 点・ベクトル

```cpp
exact_t<T> dot(Point<T> a, Point<T> b);
exact_t<T> cross(Point<T> a, Point<T> b);
exact_t<T> cross(Point<T> a, Point<T> b, Point<T> c);
exact_t<T> norm2(Point<T> p);
exact_t<T> distance2(Point<T> a, Point<T> b);
int ccw(Point<T> a, Point<T> b, Point<T> c);

Point<T> direction(Line<T> line);
Point<T> direction(Segment<T> segment);
bool is_degenerate(Line<T> line);
bool is_degenerate(Segment<T> segment);
```

`ccw` は反時計回りなら `1`、時計回りなら `-1`、共線なら `0` を返します。

`exact_t<T>` は通常の整数型に対して `i128`、`T = Rational128` に対して `Rational128` です。

## 正確な構成・交点

整数入力でも、結果が整数になるとは限らない操作は `RationalPoint` または `RationalLine` を返します。

```cpp
RationalPoint midpoint(Point<T> a, Point<T> b);
RationalPoint midpoint(Segment<T> segment);

optional<RationalPoint> line_intersection(Line<T> a, Line<U> b);
RationalPoint projection(Line<T> line, Point<U> point);
RationalPoint reflection(Line<T> line, Point<U> point);

RationalLine parallel_line(Line<T> line, Point<U> point);
RationalLine perpendicular_line(Line<T> line, Point<U> point);
RationalLine perpendicular_bisector(Point<T> a, Point<T> b);
RationalLine perpendicular_bisector(Segment<T> segment);

vector<RationalPoint> segment_intersection_points(Segment<T> a, Segment<T> b);
```

- `line_intersection`: 一意な交点を返す。平行・一致なら `nullopt`
- `projection`, `reflection`: 無限直線への射影・直線に関する鏡映
- `parallel_line`, `perpendicular_line`: 指定点を通る平行線・垂線
- `perpendicular_bisector`: 2点または線分の垂直二等分線
- `segment_intersection_points`: 交点なしなら空、1点交差なら1点、共線重複なら重複区間の両端2点

直線を作る関数には退化していない直線を渡してください。垂直二等分線には異なる2点が必要です。

## 三角形の便利関数

```cpp
exact_t<T> triangle_area2(Point<T> a, Point<T> b, Point<T> c);
RationalPoint triangle_centroid(Point<T> a, Point<T> b, Point<T> c);
optional<RationalPoint> circumcenter(Point<T> a, Point<T> b, Point<T> c);
optional<RationalPoint> orthocenter(Point<T> a, Point<T> b, Point<T> c);
```

- `triangle_area2`: 三角形の符号なし面積の2倍
- `triangle_centroid`: 重心
- `circumcenter`: 外心。3点が共線なら `nullopt`
- `orthocenter`: 垂心。3点が共線なら `nullopt`

## 平行・垂直

```cpp
bool is_parallel(A a, B b);
bool is_perpendicular(A a, B b);
```

`A`, `B` には `Line<T>` または `Segment<T>` を指定でき、両者を混ぜることもできます。方向を持たない退化した入力に対してはどちらも `false` を返します。

## 直線

```cpp
enum class LineRelation { Intersecting, Parallel, Coincident };

bool on_line(Line<T> line, Point<T> p);
LineRelation line_relation(Line<T> a, Line<T> b);
bool intersects(Line<T> a, Line<T> b);
bool intersects(Line<T> line, Segment<T> segment);
```

`intersects(Line, Line)` は交点集合が空でないかを返すため、一致する直線にも `true` を返します。直線を受け取る関数には退化していない `Line` を渡してください。退化した `Segment` は点として扱われます。

## 線分

```cpp
enum class SegmentRelation { Disjoint, Touching, Proper, Overlapping };

bool on_segment(Segment<T> segment, Point<T> p);
SegmentRelation segment_relation(Segment<T> a, Segment<T> b);
bool segments_intersect(Segment<T> a, Segment<T> b);
bool intersects(Segment<T> a, Segment<T> b);
```

| relation | meaning |
| --- | --- |
| `Disjoint` | 共通点なし |
| `Touching` | 1点で接触。端点接触や点線分を含む |
| `Proper` | 両線分の内部で横切る |
| `Overlapping` | 同一直線上で長さを持って重なる |

従来のAPIも利用できます。

```cpp
bool on_segment(Point<T> a, Point<T> b, Point<T> p);
bool segments_intersect(Point<T> a, Point<T> b, Point<T> c, Point<T> d);
```

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

bool on_circle(Circle<T> circle, Point<T> p);
bool inside_circle(Circle<T> circle, Point<T> p);
bool inside_or_on_circle(Circle<T> circle, Point<T> p);
CircleRelation circle_relation(Circle<T> a, Circle<T> b);

bool intersects(Line<T> line, Circle<T> circle);
bool intersects(Segment<T> segment, Circle<T> circle);
bool intersects(Circle<T> a, Circle<T> b);
```

`intersects` は円板ではなく円周との共通点を判定します。そのため、円の内部に完全に含まれる線分は円周と交差しません。`CircleRelation::Contained` も一方の円周が他方の内側にあり、円周同士の共通点がない状態です。`Coincident` は共通点が無限個なので `intersects` は `true` です。

円との判定では外積の2乗なども `__int128_t` で計算します。すべての中間結果が `__int128_t` に収まる必要があります。目安として、座標と半径の絶対値が `10^9` 以下なら収まります。

## 多角形・凸多角形

```cpp
vector<Point<T>> convex_hull(vector<Point<T>> points);
exact_t<T> signed_polygon_area2(const vector<Point<T>>& polygon);
exact_t<T> polygon_area2(const vector<Point<T>>& polygon);
int point_in_polygon(const vector<Point<T>>& polygon, Point<T> p);
i128 convex_diameter2(const vector<Point<T>>& polygon);
i128 lattice_points_on_boundary(const vector<Point<T>>& polygon);
```

- `convex_hull`: 重複点と辺上の共線点を除いた凸包を返す
- `polygon_area2`: 面積の2倍
- `signed_polygon_area2`: 反時計回りなら正、時計回りなら負の符号付き面積の2倍
- `point_in_polygon`: 外部 `-1`、境界 `0`、内部 `1`
- `convex_diameter2`: 凸多角形の直径の2乗
- `lattice_points_on_boundary`: 境界上の格子点数

**制約**

- `convex_diameter2` は反時計回りの凸多角形を渡す

**計算量**

- `convex_hull`: $O(N \log N)$
- その他: $O(N)$
- 点・直線・線分・円の基本判定: $O(1)$

## 使用例

```cpp
using namespace geometry_int;
using P = Point<long long>;

Line<long long> line{{0, 0}, {4, 0}};
Line<long long> parallel{{0, 1}, {4, 1}};
Segment<long long> segment{{2, -1}, {2, 1}};
Circle<long long> circle{{0, 0}, 3};

bool cross_line = intersects(line, segment);   // true
bool cross_circle = intersects(segment, circle); // false
auto relation = line_relation(line, parallel); // Parallel
auto center = midpoint(Point<long long>{0, 0}, Point<long long>{1, 1});
// center = (1/2, 1/2)

auto p = line_intersection(
    Line<long long>{{0, 0}, {2, 1}},
    Line<long long>{{0, 1}, {1, 0}}
);
// p = (2/3, 1/3)
```

## 注意

- `__int128_t` の出力には [Int128IO](../utility/Int128IO.md) が使えます。
- `convex_hull` は全点が同一直線上の場合、端点だけを返します。
- `Line` を使う判定関数には異なる2点を指定してください。退化線分は基本判定で点として扱われます。
