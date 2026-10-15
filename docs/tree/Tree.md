# Tree

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [Tree.hpp](../../libraries/tree/Tree.hpp)

重み付きの木・森を所有し、根付き構築、LCA、距離、祖先、パス、向き付きHLD区間、部分木、直径、重心を扱います。構築は反復処理で、再帰・ACLは不要です。

## include

```cpp
#include "libraries/tree/Tree.hpp"
```

## コンストラクタ

```cpp
(1) Tree<Cost> tree(int n = 0);
(2) Tree<Cost> tree(const Graph<Cost,false>& graph, int root = 0);
```

`Cost` の既定型は `long long` です。(1)は頂点だけを作り、辺追加後に `build` が必要です。(2)は入力グラフをコピーして構築します。元グラフの後の変更はTreeに影響しません。

### 制約

- $0 \leq n$
- (2)の入力は無向の森。空でなければ $0 \leq root < n$
- コストと根からのコスト和・計算中の差・パス和が `Cost` に収まる

### 計算量

- (1): $O(n)$
- (2): $O(n+m)$
- メモリ: $O(n+m)$

## add_edge / read_edges

```cpp
int tree.add_edge(int from, int to, Cost cost = Cost(1));
void tree.read_edges<Weighted = false>(int m = -1, istream& input = cin, int index_offset = 1);
void tree.read_edges<Weighted = false>(int m, int index_offset, istream& input = cin);
```

[Graph](../graph/Graph.md) と同じ意味で辺を追加します。辺追加後は、クエリの前に `build` を再実行してください。

`Weighted=false` は `u v` をコスト1として、`true` は `u v cost` を読みます。番号変換の既定は1-indexedで、`index_offset=0` にすると0-indexed入力を受け取れます。内部の頂点番号と `build` の根は常に0-indexedです。

```cpp
Tree<> tree(n);
tree.read_edges();              // n-1本、重みなし、1-indexed、cin
// tree.read_edges(-1, 0);      // n-1本、重みなし、0-indexed
// tree.read_edges<true>();     // n-1本、重み付き、1-indexed
// tree.read_edges<true>(-1, 0); // n-1本、重み付き、0-indexed
tree.build(0);
```

辺数を省略するか `m=-1` にすると `max(0,n-1)` 本を読みます。空の木・1頂点の木では入力を消費しません。既存の辺がある場合も、指定した本数を追加します。森では実際の辺数を明示してください。入力ストリームを指定する場合は `tree.read_edges<true>(m, 0, input)` とも書けます。

### 制約

- 各端点は $[0,n)$
- $m=-1$ または $0 \leq m$

### 計算量

- `add_edge`: ならし $O(1)$
- `read_edges`: 実際に読む辺数を $k$ として、ならし $O(k)$（入力読み取りを除く）

## build

```cpp
void tree.build(int root = 0);
```

指定rootの成分を最初に構築し、残る成分は未訪問の最小番号の頂点を根にします。空グラフではrootを参照しません。根を変えるときも再実行できます。

閉路・自己ループ・平行辺があれば `invalid_argument` を送出します。構築前・辺追加後のクエリは使用できません。公開配列は構築結果の参照用です。

### 制約

- 入力は無向の森。非連結・孤立点・空グラフは可
- 空でなければ $0 \leq root < n$

### 計算量

- $O(n+m)$、作業メモリ $O(n)$

## メンバ変数

| 配列 | 意味 |
| --- | --- |
| `graph` | 所有する `Graph<Cost,false>` |
| `n`, `root` | 頂点数と構築時に指定した根 |
| `parent[v]`, `parent_edge[v]` | 親と親辺ID。根は-1 |
| `depth[v]` | 根からの辺数 |
| `root_distance[v]` | 根からのコスト和 |
| `component[v]`, `roots[id]` | 成分番号と各成分の根 |
| `order` | 親が子より先に現れるDFS順。逆順で部分木DPが可能 |
| `id[v]`, `vertex[i]` | heavy-firstなDFS順序と逆写像 |
| `out[v]`, `subsize[v]` | 部分木の終端と頂点数 |
| `head[v]`, `heavy_child[v]` | heavy chainの先頭とheavy child。子なしは-1 |

## connected / lca

```cpp
bool tree.connected(int u, int v) const;
int tree.lca(int u, int v) const;
```

`connected` は同じ成分かを返します。`lca` は最小共通祖先を返し、別成分なら-1です。

### 制約

- 最新の辺集合に対して構築済み
- $0 \leq u,v < n$

### 計算量

- `connected`: $O(1)$
- `lca`: $O(\log n)$

## distance / weighted_distance

```cpp
int tree.distance(int u, int v) const;
optional<Cost> tree.weighted_distance(int u, int v) const;
```

`distance` は辺数、`weighted_distance` はコスト和を返します。別成分ならそれぞれ-1・`nullopt`。木の辺コストは負でも構いません。

### 制約

- 構築済み、$0 \leq u,v < n$
- コスト演算が `Cost` に収まる

### 計算量

- $O(\log n)$

## ancestor / level_ancestor / jump

```cpp
int tree.ancestor(int v, int k) const;
int tree.level_ancestor(int v, int d) const;
int tree.jump(int u, int v, int k) const;
```

- `ancestor`: $k$ 辺上の祖先。$k=0$ ならv
- `level_ancestor`: 絶対深さdの祖先。既存HLDと同じ意味
- `jump`: uからvへk辺進んだ頂点

位置が範囲外（負数を含む）なら-1。`jump` は別成分でも-1です。

### 制約

- 構築済み、指定した頂点は $[0,n)$

### 計算量

- $O(\log n)$

## in_subtree / subtree_range

```cpp
bool tree.in_subtree(int ancestor, int v) const;
pair<int,int> tree.subtree_range(int v) const;
```

部分木への所属を判定し、部分木の半開区間 `[id[v],out[v])` を返します。

### 制約

- 構築済み、指定した頂点は $[0,n)$

### 計算量

- $O(1)$

## path

```cpp
optional<GraphPath> tree.path(int u, int v) const;
```

頂点と辺IDをu→vの順で返します。別成分なら `nullopt`。

### 制約

- 構築済み、$0 \leq u,v < n$

### 計算量

- パス長をkとして $O(\log n+k)$、返却メモリ $O(k)$

## path_segments

```cpp
optional<vector<TreePathSegment>> tree.path_segments(int u, int v, bool vertex_query = true) const;
struct TreePathSegment { int left, right; bool reversed; };
```

u→vのパスをHLD順の半開区間 `[left,right)` に分解します。区間の並びもu→vの順です。`reversed=true` はright-1からleft、falseはleftからright-1へ読みます。順方向・逆方向の積を持つ補助構造で、非可換なパス積にも使えます。

`vertex_query=false` は辺用です。各辺を深い側の端点のidで表し、LCAの位置は含めません。辺IDは `parent_edge[vertex[i]]`。同じ頂点の辺パスは空vector、別成分は `nullopt` です。optionalは変数に保持してから走査してください。

### 制約

- 構築済み、$0 \leq u,v < n$

### 計算量

- 時間・返却区間数ともに $O(\log n)$

## diameter / centroids

```cpp
optional<TreeDiameter<Cost>> tree.diameter() const;
template<class Cost> struct TreeDiameter { Cost length; GraphPath path; };
vector<int> tree.centroids() const;
```

`diameter` は全成分を通じた最長の重み付き単純パスです。長さ0の単一頂点パスも候補なので負の辺にも対応し、全辺が非正なら長さ0、空の森なら `nullopt` です。

重心はコストを無視し、頂点を除いた各部分のサイズが元成分の半分以下になる頂点です。各成分に1個または2個あり、頂点番号順で返します。

### 制約

- 構築済み
- `diameter`: `Cost` は加算・順序比較ができ、演算結果が型に収まる

### 計算量

- `diameter`: $O(n+m)$、作業メモリ $O(n)$
- `centroids`: $O(n)$、結果のメモリ $O(n)$

## make_add_sum

```cpp
(1) TreeAddSum<long long> values = tree.make_add_sum();
(2) TreeAddSum<Value> values = tree.make_add_sum<Value>();
(3) TreeAddSum<Value> values = tree.make_add_sum(const vector<Value>& initial_values);
```

構築済みの木・森に対する、頂点値の加算・合計用オブジェクトを作ります。(1)、(2)は全頂点を0で初期化し、(3)は頂点番号順の初期値を指定します。`Value` は辺コストの `Cost` と独立です。

`TreeAddSum<Value> values(tree)`、`TreeAddSum<Value> values(tree, initial_values)` でも構築できます。HLDの配置と根付き構造をコピーして保持するため、作成元のTreeを破棄・変更・再構築しても、このオブジェクトの根や頂点集合は変わりません。新しい根や辺集合に対応するには作り直してください。

### 制約

- 作成元のTreeが最新の辺集合に対して構築済み
- (3): `initial_values.size() == tree.size()`
- `Value` は0・非負整数からの構築、コピー、加減算、`+=`、整数倍に使う乗算ができる数値型
- 加算は可換で、減算が逆操作となる。整数では値・合計・差分・添字との積などの中間値が型の範囲に収まる
- 浮動小数点型を用いる場合は加減算の丸め誤差が生じる

### 計算量

- 構築時間・保持メモリともに $O(n)$。2本のFenwick木を使い、ACLは不要

## TreeAddSum

以下の頂点番号は、作成元Treeと同じ0-indexedです。更新対象は頂点値です。辺コストと `weighted_distance` はこの更新によって変わりません。部分木は作成時の根に対する部分木で、頂点自身も含みます。

### get / set / add_vertex

```cpp
Value values.get(int v) const;
void values.set(int v, Value value);
void values.add_vertex(int v, Value delta);
```

頂点値を取得・代入・加算します。

#### 制約

- $0 \leq v < n$

#### 計算量

- $O(\log(n+2))$

### add_subtree / sum_subtree

```cpp
void values.add_subtree(int v, Value delta);
Value values.sum_subtree(int v) const;
```

`add_subtree` はvの部分木の全頂点にdeltaを加えます。`sum_subtree` はその全頂点の値の合計を返します。森では、ほかの成分に影響しません。

#### 制約

- $0 \leq v < n$

#### 計算量

- $O(\log(n+2))$

### add_path / sum_path

```cpp
bool values.add_path(int u, int v, Value delta);
optional<Value> values.sum_path(int u, int v) const;
```

uからvへのパスの両端を含む全頂点について、値を加算・合計します。同じ頂点を指定した場合はその頂点のみを対象にします。

別成分なら `add_path` は値を変更せず `false`、`sum_path` は `nullopt` を返します。同じ成分での `add_path` は `true` を返します。

#### 制約

- $0 \leq u,v < n$

#### 計算量

- $O(\log^2(n+2))$

### size / sum_all

```cpp
int values.size() const;
Value values.sum_all() const;
```

頂点数、全成分の全頂点の合計を返します。空の森ではそれぞれ0を返します。

#### 計算量

- `size`: $O(1)$
- `sum_all`: $O(\log(n+2))$

## 使用例

```cpp
#include "libraries/tree/Tree.hpp"
using namespace std;

int main() {
    Tree<> tree(3);
    tree.add_edge(0, 1, 4);
    tree.add_edge(1, 2, -2);
    tree.build(0);
    cout << tree.lca(0, 2) << '\n';                  // 0
    cout << *tree.weighted_distance(0, 2) << '\n';   // 2
    cout << tree.diameter()->length << '\n';        // 4
}
```

### 部分木への加算・合計

```cpp
#include "libraries/tree/Tree.hpp"
#include <iostream>

int main() {
    Tree<> tree(4);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(1, 3);
    tree.build(0);
    auto values = tree.make_add_sum(std::vector<long long>{1, 2, 3, 4});
    values.add_subtree(1, 10); // 頂点1,2,3に加算
    std::cout << values.sum_subtree(1) << '\n'; // 39
    values.add_path(0, 2, 2); // 頂点0,1,2に加算
    std::cout << *values.sum_path(0, 2) << '\n'; // 32
    values.set(3, -1);
    std::cout << values.get(3) << '\n';         // -1
}
```

入力から読み込む実行例と入出力形式は [tree_subtree_add_sum.cpp](../../examples/tree_subtree_add_sum.cpp) を参照してください。

## 既存ライブラリとの連携

`VirtualTree::build(selected,tree)` を利用できます。selectedは同じ成分内の頂点にしてください。既存Virtual Treeの辺距離はコスト和ではなく辺数です。

既存HLDには `tree.graph.unweighted_adjacency<long long>()` を渡せます。入力を参照として保持するライブラリには変換結果を変数に保存して渡します。

[完全な実行例](../../examples/graph_and_tree.cpp)
