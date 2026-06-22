# Tree

| library | purpose | documentation |
| --- | --- | --- |
| `HLD.hpp` | LCA、距離、祖先、パス上の頂点 | [HLD.md](HLD.md) |
| `HLD_seg.hpp` | 頂点値の一点更新・パス積 | [HLD_seg.md](HLD_seg.md) |
| `HLD_lseg.hpp` | 頂点値のパス作用・パス積 | [HLD_lseg.md](HLD_lseg.md) |
| `HLD_seg_edge.hpp` | 辺値の一点更新・パス積 | [HLD_seg_edge.md](HLD_seg_edge.md) |
| `HLD_lseg_edge.hpp` | 辺値のパス作用・パス積 | [HLD_lseg_edge.md](HLD_lseg_edge.md) |
| `CentroidDecomposition.hpp` | 重心分解木の構築 | このページ |
| `RerootingDP.hpp` | 全方位木DP | [RerootingDP.md](RerootingDP.md) |

HLDの構築は `O(N)`、パスクエリはセグメント木の操作を除いて `O(log N)` 区間に分解されます。`HLD_*seg*` はAtCoder Libraryが必要です。

## Centroid decomposition

```cpp
#include "libraries/tree/CentroidDecomposition.hpp"
```

`CentroidDecomposition cd(graph)` で構築します。`parent[v]` は重心分解木の親、`depth[v]` はその深さです。構築は `O(N log N)`、メモリは `O(N)` です。入力は無向木を想定します。

`edge.hpp` は辺値HLD用の重み付き辺型です。
