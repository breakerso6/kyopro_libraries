# Geometry

整数幾何は誤差を避けるため `__int128_t` を多用し、実数幾何は `long double` と `EPS` による判定を使います。まず整数で表せる問題なら `GeometryInt.hpp` を優先します。

## Integer geometry

`libraries/geometry/GeometryInt.hpp` は `geometry_int` 名前空間で提供されます。

```cpp
#include "libraries/geometry/GeometryInt.hpp"
```

- `Point<T>`: 座標、加減算、辞書順比較
- `dot`, `cross`, `distance2`: 戻り値は `__int128_t`
- `ccw`, `on_segment`, `segments_intersect`
- `convex_hull`: 重複点と辺の途中にある共線点を除いた反時計回りの凸包
- `polygon_area2`: 符号なし面積の2倍
- `point_in_polygon`: 外部 `-1`、境界 `0`、内部 `1`
- `convex_diameter2`: 反時計回りの凸多角形に対する直径の2乗
- `lattice_points_on_boundary`: 格子多角形の境界上の格子点数。戻り値は `__int128_t`

`libraries/geometry/ClosestPair.hpp` の `closest_pair_distance2` も同じ名前空間にあり、点が2個未満なら `-1` を返します。

整数幾何の積や距離は `__int128_t` です。出力時は必要に応じて文字列へ変換してください。

## Real geometry

`libraries/geometry/GeometryReal.hpp` は `geometry_real` 名前空間で提供され、座標型は `long double` です。

```cpp
#include "libraries/geometry/GeometryReal.hpp"
```

- 射影、反射、点と直線・線分の距離
- 線分交差と線分間距離
- 直線同士、円と直線、円同士の交点

平行または一致する直線の `line_intersection` は `nullopt`、交点が無限個になる同心円の `circle_intersections` は空配列を返します。退化した直線、つまり両端点が同じ入力は対象外です。

## Other geometry structures

- `rectangle_union_area`: 長方形を `[x1,x2) x [y1,y2)` として扱い、面積を `__int128_t` で返す
- `KDTree`: 最近傍は `{距離の2乗, 元の点ID}`、範囲検索は半開矩形

## Examples

### Convex hull

```cpp
using geometry_int::Point;

vector<Point<long long>> points = {{0, 0}, {2, 0}, {1, 1}, {0, 2}};
auto hull = geometry_int::convex_hull(points);
```

`convex_hull` は重複点と辺上の共線点を除いた凸包を、反時計回りに返します。

### Point in polygon

```cpp
int state = geometry_int::point_in_polygon(hull, {1, 1});
// outside: -1, boundary: 0, inside: 1
```
