# LowLink

`libraries/graph/LowLink.hpp` は無向グラフの橋と関節点を求めます。多重辺と非連結グラフに対応します。

## Include

```cpp
#include "libraries/graph/LowLink.hpp"
```

## コンストラクタ

```cpp
LowLink graph(int n);
```

**制約**

- $0 \leq n$

## add_edge

```cpp
int id = graph.add_edge(int u, int v);
```

無向辺を追加し、辺IDを返します。

**制約**

- $0 \leq u,v < n$

**計算量**

- $O(1)$

## build

```cpp
graph.build();
```

`articulation` と `bridges` を構築します。

**計算量**

- $O(V+E)$

## メンバ変数

| メンバ | 説明 |
| --- | --- |
| `articulation` | 関節点の頂点番号 |
| `bridges` | 橋の端点 `{u,v}` |
| `order[v]` | DFS順 |
| `low[v]` | LowLink値 |

## 注意

- `build()` 後に辺を追加した場合は、再度 `build()` を呼んでください。
