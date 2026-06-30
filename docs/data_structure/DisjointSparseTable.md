# DisjointSparseTable

`libraries/data_structure/DisjointSparseTable.hpp` は静的配列に対する一般の結合的演算の区間積を $O(1)$ で返します。

## Include

```cpp
#include "libraries/data_structure/DisjointSparseTable.hpp"
```

## コンストラクタ

```cpp
DisjointSparseTable<S, Op> table(values, op);
```

**制約**

- `Op` は結合的

**計算量**

- 構築: $O(N \log N)$

## prod

```cpp
S table.prod(int l, int r) const;
```

半開区間 `[l,r)` の積を返します。

**制約**

- $0 \leq l < r \leq N$

**計算量**

- $O(1)$

## 注意

- `SparseTable` と異なり、演算が冪等である必要はありません。
- 空区間は扱いません。
