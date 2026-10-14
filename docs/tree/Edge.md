# edge

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [edge.hpp](../../libraries/tree/edge.hpp)

重み付き辺を表す型です。辺値HLDなどの隣接リストに使います。

## include

```cpp
#include "libraries/tree/edge.hpp"
```

## コンストラクタ

```cpp
(1) edge<S> e;
(2) edge<S> e(int from, int to, S weight);
```

(1)は `from=to=-1`、`weight=S()` で構築します。(2)は指定した値をそのまま保持します。グラフへの追加や逆向き辺の生成は行いません。

### 制約

- (1): `S` を既定構築できる
- (2): `S` をコピーできる
- 頂点番号の有効性は利用するグラフ側の制約に従う

### 計算量

- $O(1)$（`S` の構築・コピーを定数時間として評価）

## メンバ変数

```cpp
int from, to;
S weight;
```

始点、終点、重みです。

## operator<

```cpp
bool e.operator<(const edge<S>& other) const;
```

`(weight,from,to)` の辞書式順序で比較します。

### 制約

- `S` が順序比較できる

### 計算量

- $O(1)$（`S` の比較を定数時間として評価）

## 使用例

```cpp
#include <iostream>
#include "libraries/tree/edge.hpp"

int main() {
    edge<long long> e(0, 1, 7);
    std::cout << e.from << ' ' << e.to << ' ' << e.weight << '\n'; // 0 1 7
}
```
