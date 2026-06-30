# HLD_lseg_edge

`libraries/tree/HLD_lseg_edge.hpp` は HLD と `atcoder::lazy_segtree` を組み合わせた、辺値用のライブラリです。各辺に値を持たせ、パスへの遅延作用、辺 ID による一点更新、パス上の積を扱えます。

`prod_path(s, t)` は `s` から `t` へのパスの辺列 `(e0, ..., ek)` に対して `op(value[e0], ..., value[ek])` を返します。非可換な演算にも対応しています。`s == t` の場合は辺を通らないため `e()` を返します。

## Include

```cpp
#include "libraries/tree/HLD_lseg_edge.hpp"
```

AtCoder Library の `atcoder/lazysegtree` が必要です。

## テンプレート引数

```cpp
template <
    class S,
    S (*op)(S, S),
    S (*e)(),
    class F,
    S (*mapping)(F, S),
    F (*composition)(F, F),
    F (*id_)()
>
struct HLD_lseg_edge;
```

`atcoder::lazy_segtree<S, op, e, F, mapping, composition, id_>` と同じ形式です。

- `S`: モノイドの型
- `op(S a, S b)`: 結合的な二項演算
- `e()`: 単位元
- `F`: 作用の型
- `mapping(F f, S x)`: `f` を `x` に作用させた結果
- `composition(F f, F g)`: `f` と `g` の合成
- `id_()`: 恒等写像

## コンストラクタ

```cpp
HLD_lseg_edge<S, op, e, F, mapping, composition, id_> hld(n, edges, root);
```

- `n`: 頂点数
- `edges`: `vector<edge<S>>`
- `root`: 根。省略時は `0`
- `edges[i]` の `from`, `to` は無向木の辺の両端、`weight` は辺 `i` の初期値です。
- `set(i, x)`、`get(i)` の `i` は `edges` に渡した順番の辺 ID です。

**制約**

- $0 \leq root < n$
- `edges` は `n` 頂点の連結な無向木
- `op` は結合的
- `e()` は `op` の単位元
- `mapping` と `composition` はACL lazy_segtreeと同じ条件を満たす

**計算量**

- 構築: `O(N log N)`
- メモリ: `O(N)`

## HLD関数

`HLD_lseg_edge` は `HLD` と同じ補助 API を持ちます。これらは頂点に対するクエリです。

| 関数 | 説明 | 計算量 |
| --- | --- | --- |
| `level_ancestor(v, d)` | `v` の祖先で深さ `d` の頂点。存在しない場合は `-1` | `O(log N)` |
| `lca(u, v)` | `u` と `v` の LCA | `O(log N)` |
| `distance(u, v)` | `u` と `v` の距離 | `O(log N)` |
| `jump(s, t, i)` | `s` から `t` へのパス上の `i` 番目の頂点 | `O(log N)` |

## 遅延セグメント木関数

### set

```cpp
void set(int edge_id, S x);
```

辺 `edge_id` の値を `x` に更新します。

**計算量**

- `O(log N)`

### get

```cpp
S get(int edge_id);
```

辺 `edge_id` の現在の値を返します。

**計算量**

- `O(log N)`

### prod_path

```cpp
S prod_path(int s, int t);
```

`s` から `t` へのパス上の辺値を、パスの順番で畳み込みます。

**計算量**

- `O(log^2 N)`

### apply_path

```cpp
void apply_path(int s, int t, F f);
```

`s` から `t` へのパス上の全辺に `f` を作用させます。`s == t` の場合は何もしません。

**計算量**

- `O(log^2 N)`

## 使用例

パス加算、パス和クエリの例です。`S` に区間長を持たせると、lazy propagation で区間全体への加算を扱えます。

```cpp
#include "libraries/tree/HLD_lseg_edge.hpp"

struct S {
    long long sum;
    int len;
};

using F = long long;

S op(S a, S b) {
    return {a.sum + b.sum, a.len + b.len};
}

S e() {
    return {0, 0};
}

S mapping(F f, S x) {
    return {x.sum + f * x.len, x.len};
}

F composition(F f, F g) {
    return f + g;
}

F id() {
    return 0;
}

int main() {
    int n = 5;
    vector<edge<S>> edges;
    edges.emplace_back(0, 1, S{10, 1});  // edge id 0
    edges.emplace_back(1, 2, S{20, 1});  // edge id 1
    edges.emplace_back(1, 3, S{30, 1});  // edge id 2
    edges.emplace_back(3, 4, S{40, 1});  // edge id 3

    HLD_lseg_edge<S, op, e, F, mapping, composition, id> hld(n, edges, 0);

    cout << hld.prod_path(2, 4).sum << '\n';  // 90

    hld.apply_path(2, 4, 10);
    cout << hld.prod_path(2, 4).sum << '\n';  // 120
}
```

## 注意

- `apply_path(s, t, f)` は辺に作用します。LCA の頂点自体には対応する辺がないため、パス上の頂点数より 1 個少ない要素が対象です。
- パス集約は非可換演算に対応していますが、`op` は結合的である必要があります。
