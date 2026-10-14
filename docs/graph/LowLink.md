# LowLink

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [LowLink.hpp](../../libraries/graph/LowLink.hpp)

`libraries/graph/LowLink.hpp` は無向グラフの橋と関節点を求めます。多重辺と非連結グラフに対応します。

## include

```cpp
#include "libraries/graph/LowLink.hpp"
```

## コンストラクタ

```cpp
LowLink graph(int n);
```

### 制約

- $0 \leq n$

## add_edge

```cpp
int id = graph.add_edge(int u, int v);
```

無向辺を追加し、辺IDを返します。

### 制約

- $0 \leq u,v < n$

### 計算量

- $O(1)$

## build

```cpp
graph.build();
```

`articulation` と `bridges` を構築します。

### 計算量

- $O(V+E)$

## メンバ変数

| メンバ | 説明 |
| --- | --- |
| `articulation` | 関節点の頂点番号 |
| `bridges` | 橋の端点 `{u,v}` |
| `order[v]` | DFS順 |
| `low[v]` | LowLink値 |

## 注意

- `build()` 後に辺を追加した場合は、再度 `build()` を呼んでください。

## 使用例

```cpp
#include "libraries/graph/LowLink.hpp"
using namespace std;

int main() {
    LowLink graph(3);
    graph.add_edge(0, 1);
    graph.add_edge(1, 2);
    graph.build();
    assert(graph.bridges.size() == 2);
    assert(graph.articulation == vector<int>{1});
}
```
