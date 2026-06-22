# HLD_seg

`libraries/tree/HLD_seg.hpp` は HLD と `atcoder::segtree` を組み合わせたライブラリです。各頂点に値を持たせ、一点更新とパス上の積を扱えます。

`prod_path(s, t)` は `s` から `t` へのパスの頂点列 `(v0, ..., vk)` に対して `op(value[v0], ..., value[vk])` を返します。非可換な演算にも対応しています。

## Include

```cpp
#include "libraries/tree/HLD_seg.hpp"
```

AtCoder Library の `atcoder/segtree` が必要です。

## Template Parameters

```cpp
template <class S, auto op, auto e>
struct HLD_seg;
```

`atcoder::segtree<S, op, e>` と同じ形式です。

- `S`: モノイドの型
- `op(S a, S b)`: 結合的な二項演算
- `e()`: 単位元

## Constructor

```cpp
HLD_seg<S, op, e> hld(graph, root);
HLD_seg<S, op, e> hld(graph, initial_values, root);
```

- `graph`: `vector<vector<ll>>`
- `initial_values`: 頂点番号順の初期値。`initial_values[v]` が頂点 `v` の値になります。
- `root`: 根。省略時は `0`
- `graph` は無向木を想定します。

計算量:

- 構築: `O(N log N)`
- メモリ: `O(N)`

## HLD API

`HLD_seg` は `HLD` と同じ補助 API を持ちます。

| function | description | complexity |
| --- | --- | --- |
| `level_ancestor(v, d)` | `v` の祖先で深さ `d` の頂点。存在しない場合は `-1` | `O(log N)` |
| `lca(u, v)` | `u` と `v` の LCA | `O(log N)` |
| `distance(u, v)` | `u` と `v` の距離 | `O(log N)` |
| `jump(s, t, i)` | `s` から `t` へのパス上の `i` 番目の頂点 | `O(log N)` |

## Segment Tree API

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

## Example

```cpp
#include "libraries/tree/HLD_seg.hpp"

long long op(long long a, long long b) {
    return a + b;
}

long long e() {
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

    vector<long long> a = {1, 2, 3, 4, 5};
    HLD_seg<long long, op, e> hld(g, a, 0);

    cout << hld.prod_path(2, 4) << '\n';  // 3 + 2 + 4 + 5 = 14

    hld.set(1, 10);
    cout << hld.prod_path(2, 4) << '\n';  // 3 + 10 + 4 + 5 = 22
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

// prod_path(u, v).value は、u から v へのパス順の文字列になります。
```
