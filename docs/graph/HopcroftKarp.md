# HopcroftKarp

`libraries/graph/HopcroftKarp.hpp` は二部グラフの最大マッチングを求めます。最大マッチング構築後、最小頂点被覆も取得できます。

## Include

```cpp
#include "libraries/graph/HopcroftKarp.hpp"
```

## コンストラクタ

```cpp
HopcroftKarp graph(int left_size, int right_size);
```

**制約**

- $0 \leq left\_size$
- $0 \leq right\_size$

## add_edge

```cpp
void graph.add_edge(int left, int right);
```

左側頂点 `left` と右側頂点 `right` の間に辺を追加します。

**制約**

- $0 \leq left < left\_size$
- $0 \leq right < right\_size$

## max_matching

```cpp
int graph.max_matching();
```

最大マッチングサイズを返します。結果は `match_left`、`match_right` に保存されます。

**計算量**

- $O(E \sqrt V)$

## minimum_vertex_cover

```cpp
auto [left, right] = graph.minimum_vertex_cover();
```

Kőnigの定理に基づき、最小頂点被覆を `{左側頂点列, 右側頂点列}` で返します。

**制約**

- `max_matching()` を呼び出し済み

**計算量**

- $O(V+E)$
