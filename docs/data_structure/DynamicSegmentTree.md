# DynamicSegmentTree

`libraries/data_structure/DynamicSegmentTree.hpp` は広い座標範囲に対する一点更新・区間積を、必要なノードだけ作って処理するセグメント木です。

座標圧縮が面倒な場合や、座標範囲は大きいが実際に更新する点が少ない場合に使います。

## Include

```cpp
#include "libraries/data_structure/DynamicSegmentTree.hpp"
```

## コンストラクタ

```cpp
DynamicSegmentTree<S, Op> seg(low, high, identity, op);
```

区間 `[low, high)` を管理します。

| parameter | meaning |
| --- | --- |
| `S` | モノイドの値型 |
| `Op` | `S operator()(S, S)` を持つ結合的な演算 |
| `identity` | 単位元 |

**制約**

- `low < high`
- `Op` は結合的
- `identity` は `Op` の単位元

**計算量**

- $O(1)$

## set

```cpp
void set(long long position, const S& value);
```

`position` の値を `value` に置き換えます。

**制約**

- `low <= position < high`

**計算量**

- $O(\log X)$

## get

```cpp
S get(long long position) const;
```

`position` の値を返します。未更新の点は `identity` です。

**制約**

- `low <= position < high`

**計算量**

- $O(\log X)$

## prod

```cpp
S prod(long long l, long long r) const;
```

半開区間 `[l, r)` の積を返します。`l = r` のときは `identity` を返します。

**制約**

- `low <= l <= r <= high`

**計算量**

- $O(\log X)$

`X = high - low` とします。

メモリは作られたノード数に比例します。1回の新規更新で最大 $O(\log X)$ ノードを追加します。

## 使用例

```cpp
struct Sum {
    long long operator()(long long a, long long b) const { return a + b; }
};

DynamicSegmentTree<long long, Sum> seg(-1000000000000LL, 1000000000000LL, 0);
seg.set(100000000000LL, 5);
seg.set(-7, 3);

cout << seg.prod(-10, 100000000001LL) << '\n'; // 8
```

## 注意

- 座標は `long long` です。
- 区間外の `set/get/prod` は `assert` に失敗します。
- 更新は代入です。加算したい場合は `get` してから `set` してください。
