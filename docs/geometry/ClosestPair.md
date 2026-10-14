# ClosestPair

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [ClosestPair.hpp](../../libraries/geometry/ClosestPair.hpp)

`libraries/geometry/ClosestPair.hpp` は整数点集合の最近点対距離の2乗を求めます。

## include

```cpp
#include "libraries/geometry/ClosestPair.hpp"
```

## 関数

```cpp
geometry_int::i128 geometry_int::closest_pair_distance2(vector<Point<T>> points);
```

点が2個未満の場合は `-1` を返します。

### 制約

- `Point<T>` は `GeometryInt.hpp` の点型

### 計算量

- $O(N \log N)$

## 注意

- 戻り値は距離そのものではなく距離の2乗です。
- 同一点が複数あれば答えは `0` です。
- 最初のソート後に重複点を検出して終了します。分割統治のマージ用バッファを帯状領域の処理にも再利用し、再帰ごとのメモリ確保を省きます。

## 使用例

```cpp
#include "libraries/geometry/ClosestPair.hpp"
using namespace std;

int main() {
    vector<geometry_int::Point<long long>> points = {{0, 0}, {3, 4}, {1, 0}};
    assert(geometry_int::closest_pair_distance2(points) == 1);
}
```
