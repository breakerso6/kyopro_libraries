# RerootingDP

```cpp
#include "libraries/tree/RerootingDP.hpp"
```

全頂点をそれぞれ根とした木DPを `O(N)` で計算します。

```cpp
auto rerooting = make_rerooting_dp<DP, EdgeData>(
    n,
    identity,
    merge,
    add_vertex,
    add_edge_dp
);
rerooting.add_edge(u, v, uv_data, vu_data);
vector<DP> answer = rerooting.build();
```

## Functions

- `merge(a,b)`: 同じ頂点へ入る寄与を隣接リスト順に結合する。結合則と単位元が必要
- `add_vertex(value,v)`: 子方向の寄与をまとめた後、頂点 `v` の情報を加える
- `add_edge_dp(value,data,from,to)`: `from` 側のDPを辺越しに `to` への寄与へ変換する

`merge` は可換でなくても構いません。prefix/suffix積により、入力した隣接リスト順を維持します。

`add_edge(u,v,data)` は両方向に同じデータを設定します。方向別のデータが必要なら `add_edge(u,v,uv_data,vu_data)` を使います。`uv_data` は頂点 `u` で `v` 側のDPを取り込む際に渡され、逆方向では `vu_data` が使われます。

入力は連結な無向木を想定します。構築時間・メモリは `O(N)` です。
