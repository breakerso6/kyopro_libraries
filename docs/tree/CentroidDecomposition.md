# CentroidDecomposition

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [CentroidDecomposition.hpp](../../libraries/tree/CentroidDecomposition.hpp)

`libraries/tree/CentroidDecomposition.hpp` は無向木から重心分解木を構築します。

重心分解は、木上の距離クエリや「条件を満たす頂点集合への最短距離」を $O(\log N)$ 個の重心経由で処理したいときに使います。

## include

```cpp
#include "libraries/tree/CentroidDecomposition.hpp"
```

## コンストラクタ

```cpp
CentroidDecomposition cd(const vector<vector<int>>& graph);
```

`graph` は `vector<vector<int>>` の無向木です。

### 制約

- `graph` は連結な無向木、または空のグラフ
- オブジェクトが保持する参照の寿命中、`graph` が存在する

## メンバ変数

| メンバ | 意味 |
| --- | --- |
| `parent[v]` | 重心分解木での親。根の重心は `-1` |
| `depth[v]` | 重心分解木での深さ |
| `subtree[v]` | 構築中に使った部分木サイズ |

## 計算量

- 構築: $O(N \log N)$
- メモリ: $O(N)$

## 使用例

```cpp
#include "libraries/tree/CentroidDecomposition.hpp"
using namespace std;

int main() {
    int n = 4;
    vector<vector<int>> g = {{1}, {0, 2, 3}, {1}, {1}};
    CentroidDecomposition cd(g);

    for (int v = 0; v < n; ++v) {
        cout << v << ' ' << cd.parent[v] << ' ' << cd.depth[v] << '\n';
    }
}
```

## 典型的な使い方

「頂点を赤く塗る更新」と「頂点 `v` から最も近い赤頂点までの距離」を処理する典型では、各頂点から重心分解木の祖先重心への距離を前計算します。

更新時:

```text
for c in centroid_ancestors[x]:
    best[c] = min(best[c], dist(x, c))
```

クエリ時:

```text
answer = min(best[c] + dist(v, c)) over c in centroid_ancestors[v]
```

各頂点の重心祖先は $O(\log N)$ 個なので、更新・クエリは $O(\log N)$ 個の候補を見るだけになります。距離 `dist` は `HLD` やLCAで求めるか、重心分解構築時に別途保存します。

## 注意

- 入力は連結な無向木を想定します。
- このクラスは分解木そのものだけを作ります。距離表や問題ごとの集約値は利用側で持ちます。
