# KDTree

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [KDTree.hpp](../../libraries/geometry/KDTree.hpp)

`libraries/geometry/KDTree.hpp` は整数点集合に対する2次元KD-treeです。最近傍探索と半開矩形範囲検索を提供します。

## include

```cpp
#include "libraries/geometry/KDTree.hpp"
```

## コンストラクタ

```cpp
KDTree<T> tree(points);
```

`points` は `geometry_int::Point<T>` の配列です。点IDは入力配列の添字です。

### 制約

- `T` は比較と減算ができる整数型

### 計算量

- 構築: 期待 $O(N \log N)$

## nearest

```cpp
pair<geometry_int::i128, int> tree.nearest(Point query) const;
```

`{距離の2乗, 点ID}` を返します。空集合なら `{-1, -1}` です。

### 計算量

- 平均的には $O(\log N)$、最悪 $O(N)$

## range_search

```cpp
vector<int> tree.range_search(T x1, T y1, T x2, T y2) const;
```

半開矩形 `[x1,x2) x [y1,y2)` に含まれる点IDを返します。

### 計算量

- 出力サイズを $K$ として平均的には $O(\sqrt N + K)$ 程度、最悪 $O(N)$

## 注意

- 戻り値の点ID順は未定義です。
- 最近傍で距離が同じ場合は点IDが小さいものを返します。

## 使用例

```cpp
#include "libraries/geometry/KDTree.hpp"
using namespace std;

int main() {
    vector<geometry_int::Point<long long>> points = {{0, 0}, {3, 4}};
    KDTree<long long> tree(points);
    assert(tree.nearest({1, 0}).second == 0);
    assert(tree.range_search(0, 0, 2, 2).size() == 1);
}
```
