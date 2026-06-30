# RerootingDP

`libraries/tree/RerootingDP.hpp` は、すべての頂点を根にした木DPをまとめて計算するための汎用全方位木DPです。

典型例は次のような問題です。

- 各頂点を根にしたときの木の高さ
- 各頂点から全頂点への距離和
- 各頂点を根にしたときの部分木情報を、辺重み込みで集約する問題

「子から親へ渡るDP値」と「辺を越えたときの変換」と「頂点での仕上げ」を関数として渡します。

## Include

```cpp
#include "libraries/tree/RerootingDP.hpp"
```

## Constructor

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

| name | meaning |
| --- | --- |
| `DP` | ある向きの根付き木が親へ渡す値 |
| `EdgeData` | 辺に持たせる値。重みなしなら `int` や空の構造体でよい |
| `identity` | `merge` の単位元 |
| `merge` | 同じ頂点に集まる複数の寄与を結合する演算 |
| `add_vertex` | 集まった寄与に頂点自身の情報を足し、DP値にする関数 |
| `add_edge_dp` | 隣接頂点のDP値を辺越しの寄与へ変換する関数 |

## Functions passed to the constructor

- `merge(a,b)`: 同じ頂点へ入る寄与を隣接リスト順に結合する。結合則と単位元が必要
- `add_vertex(value,v)`: 子方向の寄与をまとめた後、頂点 `v` の情報を加える
- `add_edge_dp(value,data,from,to)`: `from` 側のDPを辺越しに `to` への寄与へ変換する

`merge` は可換でなくても構いません。prefix/suffix積により、入力した隣接リスト順を維持します。

`add_edge(u,v,data)` は両方向に同じデータを設定します。方向別のデータが必要なら `add_edge(u,v,uv_data,vu_data)` を使います。`uv_data` は頂点 `u` で `v` 側のDPを取り込む際に渡され、逆方向では `vu_data` が使われます。

入力は連結な無向木を想定します。構築時間・メモリは `O(N)` です。

## Example: sum of distances from each vertex

`DP{size, dist}` を「その成分に含まれる頂点数」と「根からその成分内の頂点への距離和」として持ちます。子側のDPを親へ渡すとき、全頂点の距離が辺重み `w` だけ増えるので `dist += size * w` します。

```cpp
#include <bits/stdc++.h>
#include "libraries/tree/RerootingDP.hpp"
using namespace std;

struct DP {
    long long size;
    long long dist;
};

int main() {
    int n = 4;
    vector<tuple<int, int, long long>> edges = {
        {0, 1, 2},
        {1, 2, 3},
        {1, 3, 4},
    };

    auto merge = [](DP a, DP b) {
        return DP{a.size + b.size, a.dist + b.dist};
    };
    auto add_vertex = [](DP x, int) {
        return DP{x.size + 1, x.dist};
    };
    auto add_edge = [](DP x, long long w, int, int) {
        return DP{x.size, x.dist + x.size * w};
    };

    auto rerooting = make_rerooting_dp<DP, long long>(
        n, DP{0, 0}, merge, add_vertex, add_edge
    );
    for (auto [u, v, w] : edges) rerooting.add_edge(u, v, w);

    vector<DP> ans = rerooting.build();
    for (int v = 0; v < n; ++v) {
        cout << ans[v].dist << '\n';
    }
}
```

この例では `ans[v].dist` が頂点 `v` から全頂点への距離和です。

## Notes

- `merge` は結合的である必要があります。可換でなくても構いません。
- 非可換な `merge` でも、prefix/suffix を使って隣接リスト順を保ちます。
- `build(root)` の `root` は計算の開始点で、答えは全頂点について得られます。
- 入力が連結でない場合は `assert` に失敗します。
