# Geometry libraries

## Integer geometry

`libraries/geometry/GeometryInt.hpp` は `geometry_int` 名前空間で提供されます。

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

- 射影、反射、点と直線・線分の距離
- 線分交差と線分間距離
- 直線同士、円と直線、円同士の交点

平行または一致する直線の `line_intersection` は `nullopt`、交点が無限個になる同心円の `circle_intersections` は空配列を返します。退化した直線、つまり両端点が同じ入力は対象外です。

## Other geometry structures

- `rectangle_union_area`: 長方形を `[x1,x2) x [y1,y2)` として扱い、面積を `__int128_t` で返す
- `KDTree`: 最近傍は `{距離の2乗, 元の点ID}`、範囲検索は半開矩形
