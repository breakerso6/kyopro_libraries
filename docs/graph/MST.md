# MST

`libraries/graph/MST.hpp` はKruskal法で最小全域木、非連結グラフでは最小全域森を求めます。

## Include

```cpp
#include "libraries/graph/MST.hpp"
```

## コンストラクタ

```cpp
KruskalMST<T> mst(n);
```

頂点は `[0,n)` です。`T` は辺重みの型です。

**制約**

- $0 \leq n$

**計算量**

- $O(n)$

## add_edge

```cpp
int add_edge(int u, int v, T cost);
```

無向辺を追加し、追加した辺IDを返します。

**制約**

- $0 \leq u < n$
- $0 \leq v < n$

**計算量**

- $O(1)$

## build

```cpp
pair<T, vector<Edge>> build() const;
```

`{total_cost, used_edges}` を返します。非連結グラフでは最小全域森になります。

`Edge` は次のメンバを持ちます。

```cpp
struct Edge {
    int u, v;
    T cost;
    int id;
};
```

**計算量**

- $O(E \log E)$

## 使用例

```cpp
KruskalMST<long long> mst(4);
mst.add_edge(0, 1, 1);
mst.add_edge(1, 2, 5);
mst.add_edge(0, 2, 2);
mst.add_edge(2, 3, 4);

auto [cost, edges] = mst.build();
cout << cost << '\n'; // 7
```

## 注意

- 無向グラフとして扱います。
- グラフが非連結なら、各連結成分のMSTを合わせた最小全域森になります。
- 同じ重みの辺は辺IDが小さい順に見ます。
