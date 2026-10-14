# Graph

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [Graph.hpp](../../libraries/graph/Graph.hpp)

有向・無向、重み、辺IDを持つ共通のグラフ表現です。自己ループ・多重辺・孤立点に対応します。メモリは $O(V+E)$、ACLは不要です。

## include

```cpp
#include "libraries/graph/Graph.hpp"
```

## コンストラクタ

```cpp
(1) Graph<Cost, Directed> g(int n = 0);
(2) Graph<Cost, Directed> g(int n, const vector<tuple<int,int,Cost>>& edges);
```

- (1): $n$ 頂点、0辺のグラフを作ります。
- (2): 入力順に辺を追加します。次数を先に数えて隣接リストの容量を確保します。

既定は `Cost=long long`, `Directed=false` です。`Directed=true` は有向、`false` は無向です。頂点番号は $[0,n)$、辺IDは追加順の $[0,m)$ です。`Cost` は通常 `int`、`long long`、`double` などを使います。

### 制約

- $0 \leq n$
- (2): 各辺の端点は $[0,n)$
- `Cost` はコピー可能で、既定コストを使う場合は `Cost(1)` を構築できる

### 計算量

- (1): $O(n)$
- (2): $O(n+m)$

## add_edge

```cpp
int g.add_edge(int from, int to, Cost cost = Cost(1));
```

辺を追加し、辺IDを返します。無向なら逆向きのarcも追加します。両方向のarcは同じ辺IDを持ち、無向の自己ループは2arcとして次数に2を寄与します。

### 制約

- $0 \leq from,to < n$

### 計算量

- ならし $O(1)$

## read_edges

```cpp
(1) void g.read_edges<false>(int m, istream& input = cin, int index_offset = 1);
(2) void g.read_edges<true>(int m, istream& input = cin, int index_offset = 1);
```

(1)は `u v`、(2)は `u v cost` を読み、既存の辺に追加します。頂点番号から `index_offset` を引きます。既定では1-indexed入力です。(1)のコストは1です。入力を読み取れなければ `runtime_error` を送出します。

### 制約

- $0 \leq m$
- 変換後の各端点は $[0,n)$
- 入力・コスト変換を対応する型で表せる

### 計算量

- ならし $O(m)$（入力読み取りを除く）

## size / edge_count / degree / operator[]

```cpp
int g.size() const;
int g.edge_count() const;
int g.degree(int v) const;
const vector<Graph<Cost,Directed>::Edge>& g.operator[](int v) const;
```

頂点数、論理辺の本数、出次数、隣接リストを取得します。無向の `degree` は通常の次数です。

### 制約

- `degree` と `operator[]`: $0 \leq v < n$

### 計算量

- $O(1)$

## メンバ変数

```cpp
struct Edge { int from, to; Cost cost; int id; };
```

- `edges[id]`: 各論理辺が1回ずつ現れ、追加時の向き・コストを保持する
- `adj[v]`: `g[v]` と同じ隣接リスト。無向の逆向きarcは `from/to` を交換する
- `n`: 頂点数

これらは参照用です。構造の変更は `add_edge` / `read_edges` で行います。

## indegree / reversed / unweighted_adjacency

```cpp
vector<int> g.indegree() const;
Graph<Cost,Directed> g.reversed() const;
vector<vector<Index>> g.unweighted_adjacency<Index = int>() const;
```

- `indegree`: 各頂点の入次数。無向なら次数と一致します。
- `reversed`: 各論理辺の向きを反転したコピー。辺IDとコストを保持します。無向グラフの接続関係は変わりません。
- `unweighted_adjacency`: 隣接頂点だけの配列。各頂点の隣接arcの順序を保持します。

### 制約

- `Index` は頂点番号を表現できる

### 計算量

- 時間・追加メモリともに $O(n+m)$

## GraphPath

```cpp
struct GraphPath {
    vector<int> vertices;
    vector<int> edge_ids;
};
```

`vertices` は通る頂点、`edge_ids[i]` は `vertices[i]` と `vertices[i+1]` を結ぶ辺IDです。通常は頂点数が辺数より1多く、長さ0のパスは頂点1個・辺0個です。閉路は先頭と末尾が同じ頂点になります。

## 使用例

```cpp
#include "libraries/graph/Graph.hpp"
#include "libraries/tree/HLD.hpp"
using namespace std;

int main() {
    Graph<> g(3);
    g.add_edge(0, 1, 4);
    g.add_edge(1, 2, 7);
    HLD hld(g.unweighted_adjacency<long long>());
    cout << hld.distance(0, 2) << '\n'; // 2
}
```

入力を参照として保持する既存ライブラリには、隣接リストを変数に保存して渡してください。既存Dijkstra・LowLink・MSTには `g.edges` を走査して辺を追加できます。

[共通アルゴリズム](GraphAlgorithms.md) / [木・森の操作](../tree/Tree.md) / [完全な実行例](../../examples/graph_and_tree.cpp)
