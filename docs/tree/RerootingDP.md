# RerootingDP

`libraries/tree/RerootingDP.hpp` は、すべての頂点を根にした木DPをまとめて計算するための全方位木DPです。

子方向・親方向から来る寄与の型 `E`、頂点で確定したDP値の型 `V`、それらを結合・変換する関数をテンプレート引数として渡します。頂点ごとの初期値を持たせることもできます。

## Include

```cpp
#include "libraries/tree/RerootingDP.hpp"
```

## テンプレート引数

```cpp
RerootingDP<E, V, merge, e, put_edge, put_vertex> dp(n);
```

| 引数 | 入力 | 返り値 | 説明 |
| --- | --- | --- | --- |
| `E` | - | - | 隣接部分木から頂点へ渡される寄与の型 |
| `V` | - | - | 頂点で確定したDP値、または最終的な答えの型 |
| `merge` | `E a, E b` | `E` | 複数の `E` の寄与を1つの `E` に結合する関数 |
| `e` | なし | `E` | `merge` の単位元を返す関数 |
| `put_edge` | `V value, int edge_id` | `E` | 隣接頂点の `V` を、辺越しに渡す `E` へ変換する関数 |
| `put_vertex` | `E value, int vertex_id` | `V` | 集約済みの `E` を、頂点 `vertex_id` の `V` へ変換する関数 |

関数シグネチャは次の形です。

```cpp
E merge(E a, E b);
E e();
E put_edge(V value, int edge_id);
V put_vertex(E value, int vertex_id);
```

`edge_id` は `add_edge` で渡した向きごとのIDです。辺重みなどを使う場合は、外部配列を `edge_id` で参照します。

**制約**

- `merge` は結合的
- `e()` は `merge` の単位元
- 現実的には `merge` は可換であることを想定

## コンストラクタ

```cpp
RerootingDP<E, V, merge, e, put_edge, put_vertex> dp(int n);
RerootingDP<E, V, merge, e, put_edge, put_vertex> dp(int n, vector<E> initial_values);
```

- `n`: 頂点数
- `initial_values[v]`: 頂点 `v` で寄与を集約し始める初期値

`initial_values` を省略した場合、各頂点の初期値は `e()` です。頂点重みや「自分自身を候補に含める」情報をDPへ直接入れたい場合に `initial_values` を指定します。

**制約**

- $1 \leq n$
- `initial_values` を指定する場合、`initial_values.size() == n`

**計算量**

- 初期化: $O(n)$
- メモリ: $O(n)$

## add_edge

```cpp
void dp.add_edge(int u, int v, int idx, int xdi);
```

無向木の辺 `u-v` を追加します。

- `idx`: `v` 側のDP値を `u` 側へ取り込むときに `put_edge` へ渡すID
- `xdi`: `u` 側のDP値を `v` 側へ取り込むときに `put_edge` へ渡すID

無向辺で同じ重みを使う場合は、同じIDを両方に渡せます。向きごとに違う値を使う場合は、別々のIDを渡します。

**制約**

- $0 \leq u,v < n$
- 最終的に追加する辺数は `n-1`
- 追加する辺は連結な無向木をなす

**計算量**

- $O(1)$

## build

```cpp
vector<V> subdp = dp.build(int root = 0);
```

`root` を根として子方向DPを計算します。返り値は `vector<V>` です。`subdp[v]` は、この根付き木における頂点 `v` の部分木DPです。

`reroot()` を呼ぶ前に必ず一度 `build()` を呼びます。

**制約**

- $0 \leq root < n$
- `add_edge` が `n-1` 回呼ばれている

**計算量**

- $O(n)$

## reroot

```cpp
vector<V> answer = dp.reroot();
```

全方位DPを行い、`vector<V>` を返します。`answer[v]` が頂点 `v` を根としたときの答えです。

**制約**

- 先に `build(root)` を呼んでいる

**計算量**

- $O(n)$

## 使用例: 各頂点から全頂点への距離和

`E = V = pair<long long, long long>` とし、`{頂点数, 距離和}` を持ちます。辺を越えると、その成分内の各頂点までの距離が辺重み `w` だけ増えるので、`dist += size * w` します。

```cpp
#include <bits/stdc++.h>
#include "libraries/tree/RerootingDP.hpp"
using namespace std;

using DP = pair<long long, long long>;
vector<long long> weight;

DP merge(DP a, DP b) {
    return {a.first + b.first, a.second + b.second};
}
DP e() {
    return {0, 0};
}
DP put_edge(DP value, int edge_id) {
    long long w = weight[edge_id];
    value.second += value.first * w;
    return value;
}
DP put_vertex(DP value, int) {
    ++value.first;
    return value;
}

int main() {
    int n = 4;
    RerootingDP<DP, DP, merge, e, put_edge, put_vertex> dp(n);

    auto add_weighted_edge = [&](int u, int v, long long w) {
        int id = (int)weight.size();
        weight.push_back(w);
        dp.add_edge(u, v, id, id);
    };

    add_weighted_edge(0, 1, 2);
    add_weighted_edge(1, 2, 3);
    add_weighted_edge(1, 3, 4);

    dp.build(0);
    vector<DP> ans = dp.reroot();
    for (int v = 0; v < n; ++v) {
        cout << ans[v].second << '\n';
    }
}
```

## 使用例: 頂点ごとの初期値

各頂点から最も遠い頂点を、距離が同じなら頂点番号が大きいものとして求めます。`initial_values[v] = {0, v}` として、自分自身への距離を最初から候補に入れます。

```cpp
using DP = pair<int, int>; // {distance, vertex}

DP merge(DP a, DP b) { return max(a, b); }
DP e() { return {-1, -1}; }
DP put_edge(DP value, int) {
    ++value.first;
    return value;
}
DP put_vertex(DP value, int) { return value; }

vector<DP> initial_values(n);
for (int v = 0; v < n; ++v) initial_values[v] = {0, v};

RerootingDP<DP, DP, merge, e, put_edge, put_vertex> dp(n, initial_values);
```

## 注意

- `build(root)` の `root` は計算開始用の根です。`reroot()` の返り値は全頂点分得られます。
- `put_edge` に渡される `edge_id` は向きごとのIDです。同じ無向辺でも、`idx` と `xdi` に違うIDを渡せます。
- 非可換な `merge` で隣接リスト順を厳密に使う用途には向きません。
