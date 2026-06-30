# Dijkstra

`libraries/graph/Dijkstra.hpp` は非負重みグラフの単一始点最短路を求めます。

## Include

```cpp
#include "libraries/graph/Dijkstra.hpp"
```

## Constructor

```cpp
Dijkstra<T> graph(n);
```

頂点は `[0,n)` です。`T` は距離の型です。

## Methods

```cpp
void add_edge(int from, int to, T cost, bool directed = true);
vector<T> shortest_path(int source, T infinity) const;
```

- `add_edge`: 辺を追加します。`directed=false` なら無向辺として両方向に追加します。
- `shortest_path`: `source` からの距離配列を返します。到達不能頂点は `infinity` のままです。

## Complexity

- Time: `O((V + E) log V)`
- Memory: `O(V + E)`

## Example

```cpp
Dijkstra<long long> g(4);
g.add_edge(0, 1, 3);
g.add_edge(0, 2, 10);
g.add_edge(1, 2, 4);
g.add_edge(2, 3, 2);

auto dist = g.shortest_path(0, (long long)4e18);
cout << dist[3] << '\n'; // 9
```

## Notes

- 負辺がある場合は使えません。
- `d + cost` がオーバーフローしないように、十分大きい型と `infinity` を選んでください。
