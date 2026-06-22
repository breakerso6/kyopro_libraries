# HLD_seg_edge

`libraries/tree/HLD_seg_edge.hpp` は HLD と `atcoder::segtree` を組み合わせた、辺値用のライブラリです。各辺に値を持たせ、辺 ID による一点更新とパス上の積を扱えます。

`prod_path(s, t)` は `s` から `t` へのパスの辺列 `(e0, ..., ek)` に対して `op(value[e0], ..., value[ek])` を返します。非可換な演算にも対応しています。`s == t` の場合は辺を通らないため `e()` を返します。

## Include

```cpp
#include "libraries/tree/HLD_seg_edge.hpp"
```

AtCoder Library の `atcoder/segtree` が必要です。

## Template Parameters

```cpp
template <class S, auto op, auto e>
struct HLD_seg_edge;
```

`atcoder::segtree<S, op, e>` と同じ形式です。

- `S`: モノイドの型
- `op(S a, S b)`: 結合的な二項演算
- `e()`: 単位元

## Constructor

```cpp
HLD_seg_edge<S, op, e> hld(n, edges, root);
```

- `n`: 頂点数
- `edges`: `vector<edge<S>>`
- `root`: 根。省略時は `0`
- `edges[i]` の `from`, `to` は無向木の辺の両端、`weight` は辺 `i` の初期値です。
- `set(i, x)` や `get(i)` の `i` は `edges` に渡した順番の辺 ID です。

計算量:

- 構築: `O(N log N)`
- メモリ: `O(N)`

## HLD API

`HLD_seg_edge` は `HLD` と同じ補助 API を持ちます。これらは頂点に対するクエリです。

| function | description | complexity |
| --- | --- | --- |
| `level_ancestor(v, d)` | `v` の祖先で深さ `d` の頂点。存在しない場合は `-1` | `O(log N)` |
| `lca(u, v)` | `u` と `v` の LCA | `O(log N)` |
| `distance(u, v)` | `u` と `v` の距離 | `O(log N)` |
| `jump(s, t, i)` | `s` から `t` へのパス上の `i` 番目の頂点 | `O(log N)` |

## Segment Tree API

### set

```cpp
void set(int edge_id, S x);
```

辺 `edge_id` の値を `x` に更新します。

計算量: `O(log N)`

### get

```cpp
S get(int edge_id);
```

辺 `edge_id` の現在の値を返します。

計算量: `O(log N)`

### prod_path

```cpp
S prod_path(int s, int t);
```

`s` から `t` へのパス上の辺値を、パスの順番で畳み込みます。

計算量: `O(log^2 N)`

## Example

```cpp
#include "libraries/tree/HLD_seg_edge.hpp"

long long op(long long a, long long b) {
    return a + b;
}

long long e() {
    return 0;
}

int main() {
    int n = 5;
    vector<edge<long long>> edges;
    edges.emplace_back(0, 1, 10);  // edge id 0
    edges.emplace_back(1, 2, 20);  // edge id 1
    edges.emplace_back(1, 3, 30);  // edge id 2
    edges.emplace_back(3, 4, 40);  // edge id 3

    HLD_seg_edge<long long, op, e> hld(n, edges, 0);

    cout << hld.prod_path(2, 4) << '\n';  // 20 + 30 + 40 = 90

    hld.set(2, 100);
    cout << hld.prod_path(2, 4) << '\n';  // 20 + 100 + 40 = 160
}
```

## Non-Commutative Example

```cpp
struct S {
    string value;
};

S op(S a, S b) {
    return {a.value + b.value};
}

S e() {
    return {""};
}

// prod_path(u, v).value は、u から v へのパス順の辺文字列になります。
```
