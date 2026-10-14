# Dijkstra

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [Dijkstra.hpp](../../libraries/graph/Dijkstra.hpp)

`libraries/graph/Dijkstra.hpp` は非負重みグラフの単一始点最短路を求めます。

## include

```cpp
#include "libraries/graph/Dijkstra.hpp"
```

## コンストラクタ

```cpp
Dijkstra<T> graph(n);
```

頂点は `[0,n)` です。`T` は距離の型です。

### 制約

- $0 \leq n$

### 計算量

- $O(n)$

## add_edge

```cpp
void add_edge(int from, int to, T cost, bool directed = true);
```

辺を追加します。`directed=false` なら無向辺として両方向に追加します。

### 制約

- $0 \leq from < n$
- $0 \leq to < n$
- `cost` は非負

### 計算量

- ならし $O(1)$

## shortest_path

```cpp
vector<T> shortest_path(int source, T infinity) const;
```

`source` からの距離配列を返します。到達不能頂点は `infinity` のままです。

### 制約

- $0 \leq source < n$
- 全ての辺重みは非負
- 最短距離と `d + cost` が `T` の範囲に収まる
- 求める最短距離は `infinity` 未満。`infinity` 以上の距離は到達不能と区別しない

### 計算量

- $O((V + E) \log(V + E + 1))$。$V$ は頂点数、$E$ は隣接リスト内の辺数
- 単純グラフでは $O((V + E) \log(V + 1))$。多重辺ではheapに古い候補も残る

## 使用例

```cpp
Dijkstra<long long> g(4);
g.add_edge(0, 1, 3);
g.add_edge(0, 2, 10);
g.add_edge(1, 2, 4);
g.add_edge(2, 3, 2);

auto dist = g.shortest_path(0, (long long)4e18);
cout << dist[3] << '\n'; // 9
```

## 注意

- 負辺がある場合は使えません。
- `d + cost` がオーバーフローしないように、十分大きい型と `infinity` を選んでください。
