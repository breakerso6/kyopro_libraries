# LiChaoTree

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [LiChaoTree.hpp](../../libraries/data_structure/LiChaoTree.hpp)

`libraries/data_structure/LiChaoTree.hpp` は直線 `y = ax + b` の追加と、指定座標での最小値クエリを処理します。

直線を任意順に追加できます。傾き順に追加できる場合は `ConvexHullTrick.hpp` も候補です。

## include

```cpp
#include "libraries/data_structure/LiChaoTree.hpp"
```

## コンストラクタ

```cpp
LiChaoTree<T> lichao(low, high, infinity);
```

座標範囲 `[low, high)` 上でクエリします。初期状態ではすべての値が `infinity` です。

### 制約

- `low < high`
- 座標は整数幅で二分できる
- `a*x+b` が `T` に収まる

## add_line

```cpp
void lichao.add_line(T a, T b);
```

直線 `ax+b` を追加します。

### 計算量

- $O(\log X)$

## min

```cpp
T y = lichao.min(T x) const;
```

追加済み直線の `x` における最小値を返します。

### 制約

- `low <= x < high`

### 計算量

- $O(\log X)$

## 注意

- 最大値を求めたい場合は、直線と答えの符号を反転してください。
- 線分追加には対応していません。
- 同じ傾きの直線は切片だけを比較して処理します。区間の両端で既存の直線に勝てない直線は途中で破棄し、不要な子ノードの生成・走査を省きます。
- 挿入・取得は反復処理で行います。

## 使用例

```cpp
#include "libraries/data_structure/LiChaoTree.hpp"
using namespace std;

int main() {
    LiChaoTree<long long> tree(-10, 11, 1LL << 60);
    tree.add_line(2, 3);
    tree.add_line(-1, 5);
    assert(tree.min(2) == 3);
}
```
