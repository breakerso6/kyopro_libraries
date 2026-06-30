# Graph

基本的なグラフアルゴリズムを、問題でそのまま呼び出しやすい形で置いています。

| library | purpose | 計算量 |
| --- | --- | --- |
| [LowLink.hpp](LowLink.md) | 無向多重グラフの橋・関節点 | `O(V+E)` |
| [EulerianTrail.hpp](EulerianTrail.md) | 有向・無向Euler trailの構築 | `O(V+E)` |
| [HopcroftKarp.hpp](HopcroftKarp.md) | 二部最大マッチング・最小頂点被覆 | `O(E sqrt(V))` |
| [Dijkstra.hpp](Dijkstra.md) | 非負重み最短路 | `O(E log V)` |
| [MST.hpp](MST.md) | Kruskal法による最小全域森 | `O(E log E)` |
| [BiconnectedComponents.hpp](BiconnectedComponents.md) | 二重辺・二重頂点連結成分、橋木、Block-Cut Tree | `O(V+E)` |

## Include と関数

```cpp
#include "libraries/graph/LowLink.hpp"
#include "libraries/graph/EulerianTrail.hpp"
#include "libraries/graph/HopcroftKarp.hpp"
#include "libraries/graph/Dijkstra.hpp"
#include "libraries/graph/MST.hpp"
```

- `LowLink`: `add_edge(u,v)` の後に `build()`。結果は `bridges` と `articulation`。
- `EulerianTrail`: `directed(n,edges)` または `undirected(n,edges)`。不成立なら `nullopt`、成立時は頂点列と辺ID列を返します。
- `HopcroftKarp`: `add_edge(left,right)` の後に `max_matching()`。`minimum_vertex_cover()` は最大マッチング構築後に呼びます。
- `Dijkstra<T>`: `add_edge(from,to,cost,directed)` の後に `shortest_path(source, infinity)`。
- `KruskalMST<T>`: `add_edge(u,v,cost)` の後に `build()`。非連結なら最小全域森を返します。

## 使い分け

- 非負辺の単一始点最短路: [Dijkstra.hpp](Dijkstra.md)
- 無向グラフの橋・関節点: [LowLink.hpp](LowLink.md)
- 二重連結成分やBlock-Cut Treeまで欲しい: [BiconnectedComponents.hpp](BiconnectedComponents.md)
- 二部最大マッチング: [HopcroftKarp.hpp](HopcroftKarp.md)
- 最小全域木/森: [MST.hpp](MST.md)

## 二重連結成分

詳細: [BiconnectedComponents.md](BiconnectedComponents.md)

`TwoEdgeConnectedComponents` は各頂点の `component`、成分の頂点列 `groups`、橋かどうかを辺IDで表す `is_bridge`、縮約後の森 `tree` を構築します。

`VertexBiconnectedComponents` は頂点二重連結成分 `components`、関節点 `articulation`、Block-Cut Forest `block_cut_tree` を構築します。`vertex_node[v]` は元の頂点が対応するBlock-Cut Forest上のノードです。成分ノードは `[0, components.size())`、その後に関節点ノードが並びます。

どちらも `add_edge` 後に `build()` を呼びます。非連結グラフ、多重辺、孤立点に対応します。自己ループは対象外です。
