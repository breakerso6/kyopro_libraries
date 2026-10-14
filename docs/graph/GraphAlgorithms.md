# GraphAlgorithms

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [GraphAlgorithms.hpp](../../libraries/graph/GraphAlgorithms.hpp)

[Graph](Graph.md) に対する基本アルゴリズムです。DFSを含めて反復処理で実装し、再帰・ACLは不要です。頂点数を $n$、論理辺数を $m$ とします。

## include

```cpp
#include "libraries/graph/GraphAlgorithms.hpp"
```

## 探索結果

```cpp
template<class Distance> struct GraphSearchResult {
    vector<Distance> distance;
    vector<int> parent, parent_edge, order;
    vector<char> reached;
};
optional<GraphPath> result.path_to(int target) const;
```

- `distance[v]`: 始点集合からの最短距離
- `reached[v]`: 探索範囲内で到達したか
- `parent[v]`, `parent_edge[v]`: 探索木の親と辺ID。始点・未到達は-1
- `order`: 到達頂点の処理順。各頂点が1回ずつ現れる
- `path_to`: いずれかの始点からtargetまでのパス。未到達なら `nullopt`

### 制約

- `path_to`: $0 \leq target < n$

### 計算量

- `path_to`: パス長に比例
- 結果の保持メモリ: $O(n)$

## breadth_first_search

```cpp
(1) GraphSearchResult<int> breadth_first_search(g, int source);
(2) GraphSearchResult<int> breadth_first_search(g, const vector<int>& sources);
```

有向・無向のどちらでも、辺コストを無視した最小の辺数を求めます。未到達の距離は-1です。(2)は複数始点で、重複は無視し、空なら全頂点が未到達になります。

### 制約

- 各始点は $[0,n)$

### 計算量

- $O(n+m)$、作業メモリ $O(n)$
- (2)の始点列を読む時間は、追加で $O(|sources|)$

## zero_one_bfs

```cpp
(1) GraphSearchResult<int> zero_one_bfs(g, int source);
(2) GraphSearchResult<int> zero_one_bfs(g, const vector<int>& sources);
```

0/1辺コストの和を最小化します。未到達の距離は-1です。複数始点の意味はBFSと同じです。

### 制約

- 各始点は $[0,n)$
- 各辺のコストは0または1

### 計算量

- $O(n+m)$、作業メモリ $O(n)$
- (2)では追加で $O(|sources|)$

## dijkstra

```cpp
(1) GraphSearchResult<Cost> dijkstra(g, int source, Cost infinity);
(2) GraphSearchResult<Cost> dijkstra(g, const vector<int>& sources, Cost infinity);
```

有向・無向グラフの非負コストの和を最小化します。探索する距離は閉区間 $[0,infinity]$ です。範囲を超える距離と未到達の距離は `infinity` のままです。

距離がちょうど `infinity` でも到達可能なので、到達判定には `reached` を使います。加算前に残りの範囲を確認して整数のオーバーフローを避けます。

### 制約

- 各始点は $[0,n)$
- 各辺コストと `infinity` は非負
- `infinity` はグラフと同じ `Cost` 型
- `Cost` は加減算・順序比較ができる数値型。浮動小数点では有限値を用いる

### 計算量

- $O((n+m)\log(n+m+1))$、作業メモリ $O(n+m)$
- (2)では追加で $O(|sources|)$
- 単純グラフでは通常 $O((n+m)\log(n+1))$ と表せる。多重辺では古い更新候補もheapに残るため、辺数を含めた上限を用いる

## topological_sort

```cpp
optional<vector<int>> topological_sort(const Graph<Cost,true>& g);
```

全頂点のトポロジカル順を返します。閉路があれば `nullopt`、空グラフなら空vectorを返します。同じ条件を満たす順序の選択は規定しません。

### 制約

- 有向グラフ

### 計算量

- $O(n+m)$、作業メモリ $O(n)$

## strongly_connected_components

```cpp
GraphSCC strongly_connected_components(const Graph<Cost,true>& g);
```

強連結成分を求めます。結果は `size()`、`component[v]`、`groups[id]`、`dag` を持ちます。成分番号はトポロジカル順で、異なる成分を結ぶ辺では `component[from] < component[to]` が成立します。

`dag` は `Graph<int,true>` の縮約DAGです。自己ループと重複辺は除き、各辺のコストを1とします。入力のコストは使いません。

### 制約

- 有向グラフ

### 計算量

- $O(n+m)$、作業・結果のメモリ $O(n+m)$

## connected_components

```cpp
GraphComponents connected_components(const Graph<Cost,false>& g);
```

連結成分を求め、`size()`、`component[v]`、`groups[id]` を返します。孤立点も1成分として扱います。

### 制約

- 無向グラフ

### 計算量

- $O(n+m)$、作業・結果のメモリ $O(n)$

## bipartite_coloring

```cpp
optional<vector<int>> bipartite_coloring(const Graph<Cost,false>& g);
```

二部グラフなら頂点の0/1の色を返し、そうでなければ `nullopt` を返します。自己ループは二部性を壊します。空グラフでは空vectorを返します。

### 制約

- 無向グラフ

### 計算量

- $O(n+m)$、作業・結果のメモリ $O(n)$

## find_cycle

```cpp
optional<GraphPath> find_cycle(const Graph<Cost,Directed>& g);
```

有向・無向の閉路を1つ返します。なければ `nullopt`。結果は `vertices.front() == vertices.back()` を満たします。自己ループは辺1本、無向の平行辺は異なる辺ID2本の閉路として検出します。有向辺を逆向きにはたどりません。

### 計算量

- $O(n+m)$、作業メモリ $O(n)$

## 使用例

```cpp
#include "libraries/graph/GraphAlgorithms.hpp"
using namespace std;

int main() {
    Graph<long long,true> g(3);
    g.add_edge(0, 1, 2);
    g.add_edge(1, 2, 5);
    auto result = dijkstra(g, 0, 100LL);
    cout << result.distance[2] << '\n'; // 7
    auto path = result.path_to(2);
    if (path) for (int v : path->vertices) cout << v << ' '; // 0 1 2
    cout << '\n';
}
```

[木・森の操作](../tree/Tree.md) / [完全な実行例](../../examples/graph_and_tree.cpp)
