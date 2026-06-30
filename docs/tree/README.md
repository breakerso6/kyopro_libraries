# Tree

| library | purpose | documentation |
| --- | --- | --- |
| [HLD.hpp](HLD.md) | LCA、距離、祖先、パス上の頂点 | [HLD.md](HLD.md) |
| [HLD_seg.hpp](HLD_seg.md) | 頂点値の一点更新・パス積 | [HLD_seg.md](HLD_seg.md) |
| [HLD_lseg.hpp](HLD_lseg.md) | 頂点値のパス作用・パス積 | [HLD_lseg.md](HLD_lseg.md) |
| [HLD_seg_edge.hpp](HLD_seg_edge.md) | 辺値の一点更新・パス積 | [HLD_seg_edge.md](HLD_seg_edge.md) |
| [HLD_lseg_edge.hpp](HLD_lseg_edge.md) | 辺値のパス作用・パス積 | [HLD_lseg_edge.md](HLD_lseg_edge.md) |
| [CentroidDecomposition.hpp](CentroidDecomposition.md) | 重心分解木の構築 | [CentroidDecomposition.md](CentroidDecomposition.md) |
| [RerootingDP.hpp](RerootingDP.md) | 全方位木DP | [RerootingDP.md](RerootingDP.md) |
| [VirtualTree.hpp](VirtualTree.md) | 指定頂点とLCAから圧縮木を構築 | [VirtualTree.md](VirtualTree.md) |

HLDの構築は `O(N)`、パスクエリはセグメント木の操作を除いて `O(log N)` 区間に分解されます。`HLD` は `lca`、`distance`、`jump` に加え、Euler Tour上の `id/out`、`in_subtree`、`path_segments` を持ちます。`HLD_*seg*` はAtCoder Libraryが必要です。

## 重心分解

```cpp
#include "libraries/tree/CentroidDecomposition.hpp"
```

`CentroidDecomposition cd(graph)` で構築します。`parent[v]` は重心分解木の親、`depth[v]` はその深さです。構築は `O(N log N)`、メモリは `O(N)` です。入力は無向木を想定します。

詳細: [CentroidDecomposition.md](CentroidDecomposition.md)

`edge.hpp` は辺値HLD用の重み付き辺型です。

## Virtual Tree

詳細: [VirtualTree.md](VirtualTree.md)

```cpp
#include "libraries/tree/VirtualTree.hpp"
auto virtual_tree = VirtualTree::build(selected_vertices, hld);
```

既存の `HLD` と頂点集合から、必要なLCAを補ったVirtual Treeを構築します。`vertices[i]` が元の頂点番号、`parent[i]` がVirtual Tree上の親添字、`distance_to_parent[i]` が元の木での距離です。`graph` には同じ辺が無向隣接リストとして入ります。入力頂点は重複していても構いません。頂点数を `K` とすると構築は `O(K log K)` です。
