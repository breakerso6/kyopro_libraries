# MST

`libraries/graph/MST.hpp` はKruskal法で最小全域木、非連結グラフでは最小全域森を求めます。

## Include

```cpp
#include "libraries/graph/MST.hpp"
```

## Constructor

```cpp
KruskalMST<T> mst(n);
```

頂点は `[0,n)` です。`T` は辺重みの型です。

## Methods

```cpp
int add_edge(int u, int v, T cost);
pair<T, vector<Edge>> build() const;
```

`add_edge` は追加した辺IDを返します。`build` は `{total_cost, used_edges}` を返します。

`Edge` は次のメンバを持ちます。

```cpp
struct Edge {
    int u, v;
    T cost;
    int id;
};
```

## Complexity

- Time: `O(E log E)`
- Memory: `O(V + E)`

## Example

```cpp
KruskalMST<long long> mst(4);
mst.add_edge(0, 1, 1);
mst.add_edge(1, 2, 5);
mst.add_edge(0, 2, 2);
mst.add_edge(2, 3, 4);

auto [cost, edges] = mst.build();
cout << cost << '\n'; // 7
```

## Notes

- 無向グラフとして扱います。
- グラフが非連結なら、各連結成分のMSTを合わせた最小全域森になります。
- 同じ重みの辺は辺IDが小さい順に見ます。
