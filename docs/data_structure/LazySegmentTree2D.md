# LazySegmentTree2D

`libraries/data_structure/LazySegmentTree2D.hpp` は長方形加算・長方形和に特化した2次元構造です。

内部実装は4本の2次元Fenwick Treeです。用途としては遅延セグメント木の「長方形加算・長方形sum」と同じです。

## Include

```cpp
#include "libraries/data_structure/LazySegmentTree2D.hpp"
```

## コンストラクタ

```cpp
RangeAddRangeSum2D<T> seg(height, width);
RangeAddRangeSum2D<T> seg(values);
```

`values` は `vector<vector<T>>` です。

**制約**

- $0 \leq height$
- $0 \leq width$

**計算量**

- `RangeAddRangeSum2D(height, width)`: $O(HW)$
- `RangeAddRangeSum2D(values)`: $O(HW \log H \log W)$

## add

```cpp
void add(int row_l, int row_r, int column_l, int column_r, T value);
```

長方形 `[row_l,row_r) x [column_l,column_r)` に `value` を加算します。

**制約**

- $0 \leq row_l \leq row_r \leq H$
- $0 \leq column_l \leq column_r \leq W$

**計算量**

- $O(\log H \log W)$

## sum

```cpp
T sum(int row_l, int row_r, int column_l, int column_r) const;
```

長方形 `[row_l,row_r) x [column_l,column_r)` の和を返します。

**制約**

- $0 \leq row_l \leq row_r \leq H$
- $0 \leq column_l \leq column_r \leq W$

**計算量**

- $O(\log H \log W)$

## 使用例

```cpp
RangeAddRangeSum2D<long long> seg(3, 4);
seg.add(0, 2, 1, 4, 5);
seg.add(1, 3, 0, 2, 2);

cout << seg.sum(0, 3, 0, 4) << '\n'; // 5*2*3 + 2*2*2 = 38
cout << seg.sum(1, 2, 1, 2) << '\n'; // 7
```

## 注意

- すべての区間は半開区間です。
- 汎用モノイドではなく、加算と和に特化しています。
- `T` は加減算と乗算を扱える数値型を想定します。
