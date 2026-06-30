# SparseTable

`libraries/data_structure/SparseTable.hpp` は静的配列に対する冪等な区間積を $O(1)$ で返します。RMQに向いています。

## Include

```cpp
#include "libraries/data_structure/SparseTable.hpp"
```

## コンストラクタ

```cpp
SparseTable<S, Op> table(values, op);
```

**制約**

- `Op` は結合的かつ冪等

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

## 使用例

```cpp
struct Min {
    int operator()(int a, int b) const { return min(a, b); }
};

SparseTable<int, Min> st(a);
cout << st.prod(l, r) << '\n';
```
