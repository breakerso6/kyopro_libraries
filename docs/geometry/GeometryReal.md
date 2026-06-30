# GeometryReal

`libraries/geometry/GeometryReal.hpp` は `long double` 座標の2次元幾何を `geometry_real` 名前空間で提供します。

## Include

```cpp
#include "libraries/geometry/GeometryReal.hpp"
```

## 型

```cpp
struct Point { Real x, y; };
struct Line { Point a, b; };
struct Circle { Point center; Real radius; };
```

`Real` は `long double` です。

## 点・線分

```cpp
Real dot(Point a, Point b);
Real cross(Point a, Point b);
Point projection(Line l, Point p);
Point reflection(Line l, Point p);
Real distance_line_point(Line l, Point p);
Real distance_segment_point(Line s, Point p);
bool segments_intersect(Line s, Line t);
Real distance_segments(Line s, Line t);
```

**制約**

- 退化した直線・線分は対象外

**計算量**

- $O(1)$

## 交点

```cpp
optional<Point> line_intersection(Line l, Line m);
vector<Point> circle_line_intersections(Circle c, Line l);
vector<Point> circle_intersections(Circle a, Circle b);
```

交点を0個、1個、2個の配列または `optional` で返します。

**計算量**

- $O(1)$

## 注意

- 判定には `EPS` を使います。
- 一致する直線や同心円など、交点が無限個になる場合は空または `nullopt` です。
