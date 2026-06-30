# SegmentTree2D

`libraries/data_structure/SegmentTree2D.hpp` は2次元グリッド上の一点更新・長方形積を扱います。

## Include

```cpp
#include "libraries/data_structure/SegmentTree2D.hpp"
```

## コンストラクタ

```cpp
SegmentTree2D<S, Op> seg(height, width, identity, op);
SegmentTree2D<S, Op> seg(values, identity, op);
```

`values` は `vector<vector<S>>` です。すべての行の長さが同じである必要があります。

**制約**

- $0 \leq height$
- $0 \leq width$
- `Op` は結合的
- `identity` は `Op` の単位元

**計算量**

- $O(HW)$

## set

```cpp
void set(int row, int column, const S& value);
```

1マスを `value` に代入します。

**制約**

- $0 \leq row < H$
- $0 \leq column < W$

**計算量**

- $O(\log H \log W)$

## prod

```cpp
S prod(int row_l, int row_r, int column_l, int column_r) const;
```

長方形 `[row_l,row_r) x [column_l,column_r)` の積を返します。

**制約**

- $0 \leq row_l \leq row_r \leq H$
- $0 \leq column_l \leq column_r \leq W$

**計算量**

- $O(\log H \log W)$

## 使用例

```cpp
struct Sum {
    long long operator()(long long a, long long b) const { return a + b; }
};

vector<vector<long long>> a = {
    {1, 2, 3},
    {4, 5, 6},
};
SegmentTree2D<long long, Sum> seg(a, 0);

cout << seg.prod(0, 2, 1, 3) << '\n'; // 2 + 3 + 5 + 6 = 16
seg.set(1, 1, 10);
cout << seg.prod(1, 2, 0, 3) << '\n'; // 4 + 10 + 6 = 20
```

## 注意

- すべての区間は半開区間です。
- 演算は結合的である必要があります。
- メモリは `4HW` 程度を使うため、巨大な疎グリッドには向きません。
