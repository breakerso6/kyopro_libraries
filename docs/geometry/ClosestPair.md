# ClosestPair

`libraries/geometry/ClosestPair.hpp` は整数点集合の最近点対距離の2乗を求めます。

## Include

```cpp
#include "libraries/geometry/ClosestPair.hpp"
```

## 関数

```cpp
geometry_int::i128 geometry_int::closest_pair_distance2(vector<Point<T>> points);
```

点が2個未満の場合は `-1` を返します。

**制約**

- `Point<T>` は `GeometryInt.hpp` の点型

**計算量**

- $O(N \log N)$

## 注意

- 戻り値は距離そのものではなく距離の2乗です。
- 同一点が複数あれば答えは `0` です。
