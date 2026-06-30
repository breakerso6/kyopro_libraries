# GeometryInt

`libraries/geometry/GeometryInt.hpp` は整数座標の2次元幾何を `geometry_int` 名前空間で提供します。外積や距離の戻り値は `__int128_t` です。

## Include

```cpp
#include "libraries/geometry/GeometryInt.hpp"
```

## Point

```cpp
template<class T = long long>
struct Point {
    T x, y;
};
```

加算、減算、等値比較、辞書順比較を持ちます。

## 基本関数

```cpp
i128 dot(Point<T> a, Point<T> b);
i128 cross(Point<T> a, Point<T> b);
i128 cross(Point<T> a, Point<T> b, Point<T> c);
i128 norm2(Point<T> p);
i128 distance2(Point<T> a, Point<T> b);
int ccw(Point<T> a, Point<T> b, Point<T> c);
```

**計算量**

- $O(1)$

## 線分

```cpp
bool on_segment(Point<T> a, Point<T> b, Point<T> p);
bool segments_intersect(Point<T> a, Point<T> b, Point<T> c, Point<T> d);
```

**計算量**

- $O(1)$

## 多角形・凸多角形

```cpp
vector<Point<T>> convex_hull(vector<Point<T>> points);
i128 polygon_area2(const vector<Point<T>>& polygon);
int point_in_polygon(const vector<Point<T>>& polygon, Point<T> p);
i128 convex_diameter2(const vector<Point<T>>& polygon);
i128 lattice_points_on_boundary(const vector<Point<T>>& polygon);
```

- `convex_hull`: 重複点と辺上の共線点を除いた凸包を返す
- `polygon_area2`: 面積の2倍
- `point_in_polygon`: 外部 `-1`、境界 `0`、内部 `1`
- `convex_diameter2`: 凸多角形の直径の2乗
- `lattice_points_on_boundary`: 境界上の格子点数

**制約**

- `convex_diameter2` は反時計回りの凸多角形を渡す

**計算量**

- `convex_hull`: $O(N \log N)$
- その他: $O(N)$

## 注意

- `__int128_t` の出力は必要に応じて自前で文字列化してください。
- `convex_hull` は全点が同一直線上の場合、端点だけを返します。
