# RectangleUnionArea

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [RectangleUnionArea.hpp](../../libraries/geometry/RectangleUnionArea.hpp)

`libraries/geometry/RectangleUnionArea.hpp` は軸平行長方形の和集合面積を求めます。

## include

```cpp
#include "libraries/geometry/RectangleUnionArea.hpp"
```

## 関数

```cpp
__int128_t rectangle_union_area<T>(const vector<array<T, 4>>& rectangles);
```

各長方形は `{x1, y1, x2, y2}` で、半開長方形 `[x1,x2) x [y1,y2)` として扱います。

### 制約

- 座標は比較可能
- 面積が `__int128_t` に収まる

### 計算量

- $O(N \log N)$

## 注意

- `x1 >= x2` または `y1 >= y2` の退化長方形は無視します。
- 圧縮したy区間を左右のイベントで共有し、座標の二分探索を長方形ごとに2回に抑えます。区間更新では交差する子だけを走査します。

## 使用例

```cpp
#include "libraries/geometry/RectangleUnionArea.hpp"
using namespace std;

int main() {
    vector<array<long long, 4>> rectangles = {{0, 0, 2, 2}, {1, 1, 3, 3}};
    assert(rectangle_union_area(rectangles) == 7);
}
```
