# HLD_lseg

`libraries/HLD_lseg.hpp` は HLD と `atcoder::lazy_segtree` を組み合わせたライブラリです。各頂点に値を持たせ、パスへの遅延作用、一点更新、パス上の積を扱えます。

`prod_path(s, t)` は `s` から `t` へのパスの頂点列 `(v0, ..., vk)` に対して `op(value[v0], ..., value[vk])` を返します。非可換な演算にも対応しています。

## Include

```cpp
#include "libraries/HLD_lseg.hpp"
```

AtCoder Library の `atcoder/lazysegtree` が必要です。

## Template Parameters

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
struct HLD_lseg;
```

`atcoder::lazy_segtree<S, op, e, F, mapping, composition, id_>` と同じ形式です。

- `S`: モノイドの型
- `op(S a, S b)`: 結合的な二項演算
- `e()`: 単位元
- `F`: 作用の型
- `mapping(F f, S x)`: `f` を `x` に作用させた結果
- `composition(F f, F g)`: `f` と `g` の合成
- `id_()`: 恒等写像

## Constructor

```cpp
HLD_lseg<S, op, e, F, mapping, composition, id_> hld(graph, root);
HLD_lseg<S, op, e, F, mapping, composition, id_> hld(graph, initial_values, root);
```

- `graph`: `vector<vector<ll>>`
- `initial_values`: 頂点番号順の初期値。`initial_values[v]` が頂点 `v` の値になります。
- `root`: 根。省略時は `0`
- `graph` は無向木を想定します。

計算量:

- 構築: `O(N log N)`
- メモリ: `O(N)`

## HLD API

`HLD_lseg` は `HLD` と同じ補助 API を持ちます。

| function | description | complexity |
| --- | --- | --- |
| `level_ancestor(v, d)` | `v` の祖先で深さ `d` の頂点。存在しない場合は `-1` | `O(log N)` |
| `lca(u, v)` | `u` と `v` の LCA | `O(log N)` |
| `distance(u, v)` | `u` と `v` の距離 | `O(log N)` |
| `jump(s, t, i)` | `s` から `t` へのパス上の `i` 番目の頂点 | `O(log N)` |

## Lazy Segment Tree API

### set

```cpp
void set(int v, S x);
```

頂点 `v` の値を `x` に更新します。

計算量: `O(log N)`

### get

```cpp
S get(int v);
```

頂点 `v` の現在の値を返します。

計算量: `O(log N)`

### prod_path

```cpp
S prod_path(int s, int t);
```

`s` から `t` へのパス上の頂点値を、パスの順番で畳み込みます。

計算量: `O(log^2 N)`

### apply_path

```cpp
void apply_path(int s, int t, F f);
```

`s` から `t` へのパス上の全頂点に `f` を作用させます。

計算量: `O(log^2 N)`

## Example

パス加算、パス和クエリの例です。`S` に区間長を持たせると、lazy propagation で区間全体への加算を扱えます。

```cpp
#include "libraries/HLD_lseg.hpp"

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
    vector<vector<ll>> g(n);
    auto add_edge = [&](int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    };
    add_edge(0, 1);
    add_edge(1, 2);
    add_edge(1, 3);
    add_edge(3, 4);

    vector<S> a = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};
    HLD_lseg<S, op, e, F, mapping, composition, id> hld(g, a, 0);

    cout << hld.prod_path(2, 4).sum << '\n';  // 14

    hld.apply_path(2, 4, 10);
    cout << hld.prod_path(2, 4).sum << '\n';  // 54
}
```

## Notes

- `apply_path(s, t, f)` は頂点に作用します。辺に値を持たせたい場合は、子側の頂点に辺の値を載せるなどの変換が必要です。
- パス集約は非可換演算に対応していますが、`op` は結合的である必要があります。
