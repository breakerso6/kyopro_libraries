# HLD

`libraries/tree/HLD.hpp` は木を Heavy-Light Decomposition で前処理し、LCA、距離、祖先、パス上の頂点を高速に求めるライブラリです。

## Include

```cpp
#include "libraries/tree/HLD.hpp"
```

## Constructor

```cpp
HLD hld(graph, root);
```

- `graph`: `vector<vector<ll>>`
- `root`: 根。省略時は `0`
- `graph` は無向木を想定します。

計算量:

- 構築: `O(N)`
- メモリ: `O(N)`

## Public Members

| member | description |
| --- | --- |
| `vertex[i]` | HLD 順で `i` 番目の頂点 |
| `id[v]` | 頂点 `v` の HLD 順 index |
| `head[v]` | 頂点 `v` が属する heavy path の先頭 |
| `parent[v]` | 根付き木での親。根は `-1` |
| `depth[v]` | 根からの深さ |
| `subsize[v]` | 部分木サイズ |
| `heavy_child[v]` | heavy child。存在しない場合は `-1` |
| `out[v]` | Euler Tour上で部分木区間が終わる位置。部分木は `[id[v], out[v])` |
| `root` | 構築時に指定した根 |

## API

### level_ancestor

```cpp
int level_ancestor(int v, int d);
```

頂点 `v` の祖先で、深さが `d` の頂点を返します。`depth[v] < d` の場合は `-1` を返します。

計算量: `O(log N)`

### lca

```cpp
int lca(int u, int v);
```

頂点 `u` と `v` の Lowest Common Ancestor を返します。

計算量: `O(log N)`

### distance

```cpp
int distance(int u, int v);
```

頂点 `u` と `v` の間の辺数を返します。

計算量: `O(log N)`

### in_subtree

```cpp
bool in_subtree(int ancestor, int v) const;
```

根付き木で `v` が `ancestor` の部分木に含まれるかを返します。Euler Tour上の区間 `[id[ancestor], out[ancestor])` を使います。

計算量: `O(1)`

### path_segments

```cpp
vector<pair<int, int>> path_segments(int u, int v, bool vertex_query = true);
```

`u` から `v` へのパスを、HLD順の半開区間 `[l,r)` の列に分解します。各区間は `vertex` 配列上の連続区間です。

`vertex_query=false` の場合は LCA に対応する頂点を除くため、辺値を親側の頂点位置に載せるときに使えます。

計算量: `O(log N)` 個の区間を返します。

### jump

```cpp
int jump(int s, int t, int i);
```

`s` から `t` へのパス上で、`s` から数えて `i` 番目の頂点を返します。`i = 0` なら `s`、`i = distance(s, t)` なら `t` です。

`i` がパス長を超える場合の返り値は未定義です。

計算量: `O(log N)`

## Example

```cpp
#include "libraries/tree/HLD.hpp"

int main() {
    int n = 5;
    vector<vector<ll>> g(n);
    auto add_edge = [&](int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    };
    add_edge(0, 1);
    add_edge(1, 2);
    add_edge(1, 3);
    add_edge(3, 4);

    HLD hld(g, 0);

    cout << hld.lca(2, 4) << '\n';       // 1
    cout << hld.distance(2, 4) << '\n';  // 3
    cout << hld.jump(2, 4, 2) << '\n';   // 3
}
```
