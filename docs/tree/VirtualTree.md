# VirtualTree

`libraries/tree/VirtualTree.hpp` は指定された頂点と、それらを結ぶために必要なLCAだけからなる圧縮木を構築します。多数の頂点を含む元の木上で、クエリごとに小さな木DPを行う用途に向きます。

## Include

```cpp
#include "libraries/tree/HLD.hpp"
#include "libraries/tree/VirtualTree.hpp"
```

この実装はLCAとDFS順を得るため、既存の `HLD` を前処理として利用します。

## build

```cpp
VirtualTree tree = VirtualTree::build(selected_vertices, hld);
```

- `selected_vertices`: Virtual Treeに含めたい元の頂点番号。重複可
- `hld`: 元の木を同じ根で構築した `HLD` インスタンス
- 返り値: 必要なLCAを追加したVirtual Tree

異なる指定頂点数を `K` とすると、Virtual Treeの頂点数は高々 `2K-1` です。

- 構築: `O(K log K)`
- メモリ: `O(K)`

空の頂点集合を渡した場合は、すべての配列が空のVirtual Treeを返します。

## Public Members

| member | description |
| --- | --- |
| `vertices[i]` | Virtual Tree上の添字 `i` に対応する元の頂点番号 |
| `parent[i]` | Virtual Tree上の親添字。根は `-1` |
| `distance_to_parent[i]` | 元の木における親までの辺数。根は `0` |
| `graph[i]` | `{隣接するVirtual Tree添字, 元の木での距離}` |

`vertices` はHLDのDFS順に並び、根は添字 `0` です。`parent[i] < i` なので、添字の逆順で子から親へのDPを行えます。

## Example

```cpp
#include <bits/stdc++.h>
#include "libraries/tree/HLD.hpp"
#include "libraries/tree/VirtualTree.hpp"
using namespace std;

int main() {
    int n = 7;
    vector<vector<long long>> graph(n);
    auto add_edge = [&](int u, int v) {
        graph[u].push_back(v);
        graph[v].push_back(u);
    };
    add_edge(0,1); add_edge(0,2);
    add_edge(1,3); add_edge(1,4);
    add_edge(2,5); add_edge(2,6);

    HLD hld(graph, 0);
    VirtualTree tree = VirtualTree::build(vector<int>{3,4,6}, hld);

    for (int i = 0; i < (int)tree.vertices.size(); ++i) {
        cout << "original vertex = " << tree.vertices[i]
             << ", parent index = " << tree.parent[i]
             << ", distance = " << tree.distance_to_parent[i] << '\n';
    }
}
```

この例では指定頂点 `3,4,6` に加え、必要なLCA `1,0` が自動的に追加されます。

## Tree DP Pattern

```cpp
vector<long long> dp(tree.vertices.size(), 0);
for (int i = (int)tree.vertices.size() - 1; i > 0; --i) {
    int p = tree.parent[i];
    // dp[i] と distance_to_parent[i] を使って親へ寄与を渡す
    dp[p] += dp[i];
}
```

元の頂点番号が必要な処理では常に `tree.vertices[i]` を参照します。`parent` や `graph` 内の番号は元の頂点番号ではなく、Virtual Tree上の添字です。

## Notes

- 入力は `hld` の構築に使った連結な無向木上の頂点である必要があります。
- 辺重みは扱わず、距離は元の木の辺数です。重み付き距離が必要なら、根からの累積距離を別途用意して `vertices` 間の差を計算してください。
- `selected_vertices` 自体は変更されません。値渡しされたコピーが内部で整列されます。
