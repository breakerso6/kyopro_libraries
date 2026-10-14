# IntervalSet

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [IntervalSet.hpp](../../libraries/data_structure/IntervalSet.hpp)

`libraries/data_structure/IntervalSet.hpp` は整数上の半開区間集合を管理します。重なる区間は自動でマージされます。

## include

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

### 制約

- `l <= r`

### 計算量

- 操作前の区間数を $N$、変更される区間数を $K$ として $O((K+1)\log(N+2))$

## contains / covers / mex

```cpp
bool set.contains(T x) const;
bool set.covers(T l, T r) const;
T set.mex(T x) const;
```

- `contains(x)`: `x` が集合に含まれるか
- `covers(l,r)`: `[l,r)` がすべて集合に含まれるか
- `mex(x)`: `x` 以上で集合に含まれない最小値

### 計算量

- $O(\log N)$

## 注意

- 区間はすべて半開区間 `[l,r)` です。
- 空区間の `insert/erase` は集合を変えません。

## 使用例

```cpp
#include "libraries/data_structure/IntervalSet.hpp"
using namespace std;

int main() {
    IntervalSet<int> intervals;
    intervals.insert(2, 5);
    intervals.insert(5, 7);
    assert(intervals.covers(2, 7));
    intervals.erase(4, 6);
    assert(intervals.mex(2) == 4);
}
```
