# BiconnectedComponents

`libraries/graph/BiconnectedComponents.hpp` は無向グラフの二重辺連結成分と二重頂点連結成分を構築します。橋木とBlock-Cut Forestも同時に得られます。

**制約**

- 頂点番号は `0`-indexed
- 自己ループは対象外

## Include

```cpp
#include "libraries/graph/BiconnectedComponents.hpp"
```

2種類の構造は独立しているため、必要な方へ同じ辺を登録してください。

## TwoEdgeConnectedComponents

橋を取り除いても互いに到達できる頂点を1成分にまとめます。

### コンストラクタ・辺追加

```cpp
TwoEdgeConnectedComponents graph(n);
int edge_id = graph.add_edge(u, v);
graph.build();
```

`add_edge` は0から追加順に辺IDを返します。`build()` の計算量は `O(V+E)`、メモリも `O(V+E)` です。

**制約**

- $0 \leq u,v < n$

### メンバ変数

| メンバ | 説明 |
| --- | --- |
| `edges[id]` | 辺IDに対応する端点 `{u,v}` |
| `component[v]` | 頂点 `v` の二重辺連結成分ID |
| `groups[id]` | 成分IDに含まれる元の頂点列 |
| `is_bridge[id]` | 辺が橋なら `true` |
| `tree[id]` | 成分を縮約した橋森の隣接リスト |

入力が連結なら `tree` は木、非連結なら森になります。

## VertexBiconnectedComponents

関節点で分離されない極大な頂点集合をBlockに分解し、Blockと関節点からなる二部森を構築します。

### コンストラクタ・辺追加

```cpp
VertexBiconnectedComponents graph(n);
int edge_id = graph.add_edge(u, v);
graph.build();
```

`build()` の計算量・メモリは `O(V+E)` です。

### メンバ変数

| member | description |
| --- | --- |
| `components[id]` | Block `id` に含まれる元の頂点列 |
| `articulation` | 関節点を頂点番号の昇順に並べた列 |
| `block_cut_tree` | Block-Cut Forestの隣接リスト |
| `vertex_node[v]` | 元の頂点 `v` が対応するForest上のノード |

Block-Cut Forestのノード番号は次の規約です。

- `[0, components.size())`: Blockノード
- `[components.size(), block_cut_tree.size())`: `articulation` と同じ順の関節点ノード

非関節点はただ1つのBlockに属するため、`vertex_node[v]` はそのBlockノードです。関節点は複数Blockに属するため、専用の関節点ノードを指します。孤立点は、その頂点だけを含むBlockになります。

## 使用例

```cpp
#include <bits/stdc++.h>
#include "libraries/graph/BiconnectedComponents.hpp"
using namespace std;

int main() {
    vector<pair<int,int>> edges = {
        {0,1}, {1,2}, {2,0}, // triangle
        {1,3},               // bridge
        {3,4}, {3,4}         // parallel edges
    };

    TwoEdgeConnectedComponents two(5);
    VertexBiconnectedComponents vertex(5);
    for (auto [u,v] : edges) {
        two.add_edge(u,v);
        vertex.add_edge(u,v);
    }
    two.build();
    vertex.build();

    for (int id = 0; id < (int)edges.size(); ++id)
        if (two.is_bridge[id]) cout << "bridge " << id << '\n'; // edge 3

    for (int v : vertex.articulation) cout << v << ' '; // 1 3
    cout << '\n';
}
```

## 注意

- 無向グラフ専用です。
- 非連結グラフ、孤立点、多重辺に対応します。平行辺2本は橋になりません。
- 自己ループは対象外で、`add_edge(u,u)` はassertに失敗します。
- `build()` 後に辺を追加した場合は、結果を更新するため再度 `build()` を呼んでください。
- 頂点二重連結成分では関節点が複数の `components` に現れるため、`components` は頂点の分割ではありません。
