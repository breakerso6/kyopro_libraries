# IntervalSet

`libraries/data_structure/IntervalSet.hpp` は整数上の半開区間集合を管理します。重なる区間は自動でマージされます。

## Include

```cpp
#include "libraries/data_structure/IntervalSet.hpp"
```

## コンストラクタ

```cpp
IntervalSet<T> set;
```

## insert / erase

```cpp
void set.insert(T l, T r);
void set.erase(T l, T r);
```

半開区間 `[l,r)` を追加・削除します。

**制約**

- `l <= r`

**計算量**

- 変更される区間数を $K$ として $O(K \log N)$

## contains / covers / mex

```cpp
bool set.contains(T x) const;
bool set.covers(T l, T r) const;
T set.mex(T x) const;
```

- `contains(x)`: `x` が集合に含まれるか
- `covers(l,r)`: `[l,r)` がすべて集合に含まれるか
- `mex(x)`: `x` 以上で集合に含まれない最小値

**計算量**

- $O(\log N)$

## 注意

- 区間はすべて半開区間 `[l,r)` です。
- 空区間の `insert/erase` は集合を変えません。
