# Geometry

整数幾何は誤差を避けるため `__int128_t` と既約分数 `Rational` を使い、実数幾何は `long double` と `EPS` による判定を使います。まず入力が整数・有理数で表せる問題なら [GeometryInt.hpp](GeometryInt.md) を優先します。

## 整数幾何

`libraries/geometry/GeometryInt.hpp` は `geometry_int` 名前空間で提供されます。

```cpp
#include "libraries/geometry/GeometryInt.hpp"
```

- `Point<T>`, `Line<T>`, `Segment<T>`, `Circle<T>`: 点・無限直線・閉線分・円周
- `Rational128`, `RationalPoint`: [math/Rational.hpp](../math/Rational.md) の `Rational<__int128_t>` による正確な有理数・有理点
- `dot`, `cross`, `distance2`: 整数座標では `__int128_t`、有理座標では `Rational`
- `is_parallel`, `is_perpendicular`, `line_relation`, `segment_relation`, `circle_relation`
- 型を組み合わせた `intersects`、`on_line`, `on_segment`, `on_circle`
- 中点、直線・線分交点、射影、反射、平行線、垂線、垂直二等分線
- 三角形の重心・外心・垂心
- `convex_hull`: 重複点と辺の途中にある共線点を除いた反時計回りの凸包
- `polygon_area2`: 符号なし面積の2倍
- `point_in_polygon`: 外部 `-1`、境界 `0`、内部 `1`
- `convex_diameter2`: 反時計回りの凸多角形に対する直径の2乗
- `lattice_points_on_boundary`: 格子多角形の境界上の格子点数。戻り値は `__int128_t`

詳細: [GeometryInt.md](GeometryInt.md)

`libraries/geometry/ClosestPair.hpp` の `closest_pair_distance2` も同じ名前空間にあり、点が2個未満なら `-1` を返します。詳細: [ClosestPair.md](ClosestPair.md)

整数幾何の積や距離は `__int128_t` です。出力時は [Int128IO](../utility/Int128IO.md) が使えます。

## 実数幾何

`libraries/geometry/GeometryReal.hpp` は `geometry_real` 名前空間で提供され、座標型は `long double` です。

```cpp
#include "libraries/geometry/GeometryReal.hpp"
```

- `Point`, `Line`, `Segment`, `Circle` の明示型
- 平行・垂直、直線・線分・円周の関係分類と交差判定
- 射影、反射、点と直線・線分の距離
- 直線同士、円と直線・線分、円同士の交点

`Line` は無限直線、`Segment` は閉線分、`Circle` は円周を表します。平行または一致する直線の `line_intersection` は `nullopt`、交点が無限個になる同心円の `circle_intersections` は空配列を返します。退化した直線、つまり両端点が同じ入力は対象外です。

詳細: [GeometryReal.md](GeometryReal.md)

## その他の幾何構造

- [rectangle_union_area](RectangleUnionArea.md): 長方形を `[x1,x2) x [y1,y2)` として扱い、面積を `__int128_t` で返す
- [KDTree](KDTree.md): 最近傍は `{距離の2乗, 元の点ID}`、範囲検索は半開矩形

## 使用例

### 凸包

```cpp
using geometry_int::Point;

vector<Point<long long>> points = {{0, 0}, {2, 0}, {1, 1}, {0, 2}};
auto hull = geometry_int::convex_hull(points);
```

`convex_hull` は重複点と辺上の共線点を除いた凸包を、反時計回りに返します。

### 点の内外判定

```cpp
int state = geometry_int::point_in_polygon(hull, {1, 1});
// outside: -1, boundary: 0, inside: 1
```
