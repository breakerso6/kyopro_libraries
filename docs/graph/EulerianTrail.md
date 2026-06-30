# EulerianTrail

`libraries/graph/EulerianTrail.hpp` は有向・無向グラフのEuler trailを構築します。

## Include

```cpp
#include "libraries/graph/EulerianTrail.hpp"
```

## undirected

```cpp
optional<EulerianTrail::Result> EulerianTrail::undirected(
    int n,
    const vector<pair<int, int>>& edges
);
```

無向グラフで全ての辺をちょうど1回通るtrailを返します。存在しなければ `nullopt` です。

**制約**

- $0 \leq n$
- 各辺 `(u,v)` は $0 \leq u,v < n$

**計算量**

- $O(V+E)$

## directed

```cpp
optional<EulerianTrail::Result> EulerianTrail::directed(
    int n,
    const vector<pair<int, int>>& edges
);
```

有向グラフで全ての辺をちょうど1回通るtrailを返します。存在しなければ `nullopt` です。

**計算量**

- $O(V+E)$

## Result

```cpp
struct Result {
    vector<int> vertices;
    vector<int> edge_ids;
};
```

`vertices` は通る頂点列、`edge_ids` は通る辺ID列です。成立時は `vertices.size() == edges.size() + 1`、`edge_ids.size() == edges.size()` です。

## 注意

- 孤立点はtrailの存在判定に影響しません。
- 辺が空の場合は頂点数が正なら頂点列1個のtrailを返します。
