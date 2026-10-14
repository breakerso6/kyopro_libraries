# HopcroftKarp

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [HopcroftKarp.hpp](../../libraries/graph/HopcroftKarp.hpp)

`libraries/graph/HopcroftKarp.hpp` は二部グラフの最大マッチングを求めます。最大マッチング構築後、最小頂点被覆も取得できます。

## include

```cpp
#include "libraries/graph/HopcroftKarp.hpp"
```

## コンストラクタ

```cpp
HopcroftKarp graph(int left_size, int right_size);
```

### 制約

- $0 \leq left\_size$
- $0 \leq right\_size$

## add_edge

```cpp
void graph.add_edge(int left, int right);
```

左側頂点 `left` と右側頂点 `right` の間に辺を追加します。

### 制約

- $0 \leq left < left\_size$
- $0 \leq right < right\_size$

## max_matching

```cpp
int graph.max_matching();
```

最大マッチングサイズを返します。結果は `match_left`、`match_right` に保存されます。

最初に未使用の右頂点への直接増加を行い、残った増加路だけを層状に探索します。BFSは最短の増加路までの層に限定し、DFSは各頂点の探索済み辺をカーソルで管理します。マッチングが左右の小さい側の頂点数に達したら終了します。辺を追加して再実行する場合も、既存マッチングを引き継ぎます。

### 計算量

- $O(E \sqrt V)$

## minimum_vertex_cover

```cpp
auto [left, right] = graph.minimum_vertex_cover();
```

Kőnigの定理に基づき、最小頂点被覆を `{左側頂点列, 右側頂点列}` で返します。

### 制約

- `max_matching()` を呼び出し済み

### 計算量

- $O(V+E)$

## 使用例

```cpp
#include "libraries/graph/HopcroftKarp.hpp"
using namespace std;

int main() {
    HopcroftKarp graph(2, 2);
    graph.add_edge(0, 0);
    graph.add_edge(0, 1);
    graph.add_edge(1, 0);
    assert(graph.max_matching() == 2);
    auto cover = graph.minimum_vertex_cover();
    assert(cover.first.size() + cover.second.size() == 2);
}
```
