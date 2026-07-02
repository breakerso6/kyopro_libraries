# DynamicSegmentTree

`libraries/data_structure/DynamicSegmentTree.hpp` は広い座標範囲に対する一点更新・区間積を、必要なノードだけ作って処理するセグメント木です。

座標圧縮が面倒な場合や、座標範囲は大きいが実際に更新する点が少ない場合に使います。

## Include

```cpp
#include "libraries/data_structure/DynamicSegmentTree.hpp"
```

## コンストラクタ

```cpp
DynamicSegmentTree<S, op, e> seg(low, high);
```

区間 `[low, high)` を管理します。

| parameter | meaning |
| --- | --- |
| `S` | モノイドの値型 |
| `op` | `S op(S, S)` 形式の結合的な演算 |
| `e` | 単位元を返す `S e()` |

**制約**

- `low < high`
- `op` は結合的
- `e()` は `op` の単位元

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

`position` の値を返します。未更新の点は `e()` です。

**制約**

- `low <= position < high`

**計算量**

- $O(\log X)$

## prod

```cpp
S prod(long long l, long long r) const;
```

半開区間 `[l, r)` の積を返します。`l = r` のときは `e()` を返します。

**制約**

- `low <= l <= r <= high`

**計算量**

- $O(\log X)$

`X = high - low` とします。

メモリは作られたノード数に比例します。1回の新規更新で最大 $O(\log X)$ ノードを追加します。

## 使用例

```cpp
long long op(long long a, long long b) {
    return a + b;
}

long long e() {
    return 0;
}

DynamicSegmentTree<long long, op, e> seg(-1000000000000LL, 1000000000000LL);
seg.set(100000000000LL, 5);
seg.set(-7, 3);

cout << seg.prod(-10, 100000000001LL) << '\n'; // 8
```

## 注意

- 座標は `long long` です。
- 区間外の `set/get/prod` は `assert` に失敗します。
- 更新は代入です。加算したい場合は `get` してから `set` してください。
