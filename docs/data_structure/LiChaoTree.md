# LiChaoTree

`libraries/data_structure/LiChaoTree.hpp` は直線 `y = ax + b` の追加と、指定座標での最小値クエリを処理します。

直線を任意順に追加できます。傾き順に追加できる場合は `ConvexHullTrick.hpp` も候補です。

## Include

```cpp
#include "libraries/data_structure/LiChaoTree.hpp"
```

## コンストラクタ

```cpp
LiChaoTree<T> lichao(low, high, infinity);
```

座標範囲 `[low, high)` 上でクエリします。初期状態ではすべての値が `infinity` です。

**制約**

- `low < high`
- 座標は整数幅で二分できる
- `a*x+b` が `T` に収まる

## add_line

```cpp
void lichao.add_line(T a, T b);
```

直線 `ax+b` を追加します。

**計算量**

- $O(\log X)$

## min

```cpp
T y = lichao.min(T x) const;
```

追加済み直線の `x` における最小値を返します。

**制約**

- `low <= x < high`

**計算量**

- $O(\log X)$

## 注意

- 最大値を求めたい場合は、直線と答えの符号を反転してください。
- 線分追加には対応していません。
