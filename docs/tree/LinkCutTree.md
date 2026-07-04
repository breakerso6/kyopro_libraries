# LinkCutTree

`libraries/tree/LinkCutTree.hpp` は動的森を管理し、辺の追加・削除、連結判定、根の変更、パス積をならし `O(log N)` で行うデータ構造です。

## Include

```cpp
#include "libraries/tree/LinkCutTree.hpp"
```

## テンプレート引数

```cpp
template<class S,
         S (*op)(S, S),
         S (*e)(),
         class F = link_cut_tree_detail::NoLazy,
         S (*mapping)(F, S) = link_cut_tree_detail::default_mapping<S>,
         F (*composition)(F, F) = link_cut_tree_detail::default_composition,
         F (*id_)() = link_cut_tree_detail::default_id>
struct LinkCutTree;
```

| parameter | meaning |
| --- | --- |
| `S` | 頂点値・パス積の型 |
| `op` | `S op(S, S)` 形式の結合的な演算 |
| `e` | 単位元を返す `S e()` |
| `F` | 遅延作用の型 |
| `mapping` | `S mapping(F, S)` 形式で、作用 `F` をパス積 `S` に適用する関数 |
| `composition` | `F composition(F f, F g)` 形式で、`g` の後に `f` を適用する合成関数 |
| `id_` | 恒等作用を返す `F id_()` |

**制約**

- `op` は結合的
- `e()` は `op` の単位元
- `mapping(id_(), x) = x`
- `mapping(f, op(x, y)) = op(mapping(f, x), mapping(f, y))`
- `mapping(composition(f, g), x) = mapping(f, mapping(g, x))`

非可換な `op` にも対応します。`prod(u, v)` は `u` から `v` へ向かう順序で積を計算します。
`F, mapping, composition, id_` を省略した場合、パス作用は恒等作用だけになります。

## コンストラクタ

```cpp
LinkCutTree<S, op, e> lct(n);
LinkCutTree<S, op, e> lct(values);
```

`n` を渡した場合、各頂点値は `e()` で初期化されます。`values` を渡した場合、`values[i]` が頂点 `i` の値になります。

**計算量**

- `O(N)`

## 頂点値

```cpp
void set(int v, const S& value);
S get(int v);
```

頂点 `v` の値を更新・取得します。

**制約**

- $0 \leq v < N$

**計算量**

- ならし `O(log N)`

## 動的森操作

```cpp
bool connected(int u, int v);
bool link(int u, int v);
bool cut(int u, int v);
void evert(int v);
int root(int v);
int lca(int u, int v);
```

| function | meaning |
| --- | --- |
| `connected` | `u` と `v` が同じ木に属するかを返す |
| `link` | 異なる木の `u` と `v` の間に辺を追加する。追加できたら `true` |
| `cut` | 辺 `(u,v)` を削除する。削除できたら `true` |
| `evert` | `v` を属する木の根にする |
| `root` | `v` が属する木の根を返す |
| `lca` | 現在の根付き木での LCA を返す。非連結なら `-1` |

**制約**

- $0 \leq u,v < N$
- `link(u, v)` は `u` と `v` が非連結のときだけ辺を追加します。
- `cut(u, v)` は辺 `(u,v)` が存在するときだけ削除します。

**計算量**

- ならし `O(log N)`

## パスクエリ

```cpp
S prod(int u, int v);
void apply(int u, int v, F f);
```

`u` から `v` へ向かうパス上の頂点値の積を返します。
`apply` は同じパス上の各頂点に作用 `f` を遅延適用します。

**制約**

- $0 \leq u,v < N$
- `u` と `v` は連結

**計算量**

- ならし `O(log N)`

## 使用例

```cpp
#include <bits/stdc++.h>
#include "libraries/tree/LinkCutTree.hpp"
using namespace std;

long long op(long long a, long long b) {
    return a + b;
}

long long e() {
    return 0;
}

int main() {
    LinkCutTree<long long, op, e> lct(vector<long long>{1, 2, 3, 4});

    lct.link(0, 1);
    lct.link(1, 2);
    cout << lct.prod(0, 2) << '\n'; // 6

    lct.set(1, 10);
    cout << lct.prod(0, 2) << '\n'; // 14

    lct.cut(1, 2);
    cout << lct.connected(0, 2) << '\n'; // 0

    lct.link(2, 3);
    lct.link(1, 3);
    lct.evert(2);
    cout << lct.lca(0, 3) << '\n';
}
```

パス加算・パス和の例です。`S` にパス長を持たせると、lazy propagation でパス全体への加算を扱えます。

```cpp
#include <bits/stdc++.h>
#include "libraries/tree/LinkCutTree.hpp"
using namespace std;

struct S {
    long long sum;
    int size;
};

S op(S a, S b) {
    return {a.sum + b.sum, a.size + b.size};
}

S e() {
    return {0, 0};
}

S mapping(long long f, S x) {
    return {x.sum + f * x.size, x.size};
}

long long composition(long long f, long long g) {
    return f + g;
}

long long id() {
    return 0;
}

int main() {
    vector<S> a = {{1, 1}, {2, 1}, {3, 1}, {4, 1}};
    LinkCutTree<S, op, e, long long, mapping, composition, id> lct(a);

    lct.link(0, 1);
    lct.link(1, 2);
    lct.apply(0, 2, 10);              // path values: {11,12,13}

    cout << lct.prod(0, 2).sum << '\n'; // 36
}
```

## 注意

- 頂点値のみを扱います。辺値を扱う場合は、辺を表す追加頂点を作って接続します。
- `prod(u, v)` は内部で `u` を根にします。根付き木としての `lca` を使う場合は、必要に応じて `evert(root)` で根を設定してください。
- `apply(u, v, f)` も内部で `u` を根にします。
- `apply` と非可換な `op` を併用する場合、`mapping` は正順・逆順どちらのパス積にも正しく作用する必要があります。
- Splay Tree を内部で使うため、各操作の計算量はならしです。
