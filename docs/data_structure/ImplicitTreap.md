# ImplicitTreap

`libraries/data_structure/ImplicitTreap.hpp` は列を平衡二分木として管理し、位置指定の挿入・削除・区間反転・区間作用・区間積を行うデータ構造です。

## Include

```cpp
#include "libraries/data_structure/ImplicitTreap.hpp"
```

## テンプレート引数

```cpp
template<class S,
         S (*op)(S, S),
         S (*e)(),
         class F = implicit_treap_detail::NoLazy,
         S (*mapping)(F, S) = implicit_treap_detail::default_mapping<S>,
         F (*composition)(F, F) = implicit_treap_detail::default_composition,
         F (*id_)() = implicit_treap_detail::default_id>
struct ImplicitTreap;
```

| parameter | meaning |
| --- | --- |
| `S` | 要素・区間積の型 |
| `op` | `S op(S, S)` 形式の結合的な演算 |
| `e` | 単位元を返す `S e()` |
| `F` | 遅延作用の型 |
| `mapping` | `S mapping(F, S)` 形式で、作用 `F` を区間積 `S` に適用する関数 |
| `composition` | `F composition(F f, F g)` 形式で、`g` の後に `f` を適用する合成関数 |
| `id_` | 恒等作用を返す `F id_()` |

**制約**

- `op` は結合的
- `e()` は `op` の単位元
- `mapping(id_(), x) = x`
- `mapping(f, op(x, y)) = op(mapping(f, x), mapping(f, y))`
- `mapping(composition(f, g), x) = mapping(f, mapping(g, x))`

`F, mapping, composition, id_` を省略した場合、区間作用は恒等作用だけになります。既存の区間反転・区間積だけの用途では3引数のまま使えます。

## コンストラクタ

```cpp
ImplicitTreap<S, op, e> treap;
ImplicitTreap<S, op, e> treap(values);
```

`values` を渡した場合は、先頭から順に列を構築します。

**計算量**

- 空構築: `O(1)`
- `values` から構築: 期待 `O(N log N)`

## 関数

### size / empty

```cpp
int size() const;
bool empty() const;
```

列の長さ、空判定を返します。

**計算量**

- `O(1)`

### insert / erase

```cpp
void insert(int position, const S& value);
void erase(int position);
```

`insert` は `position` 番目の直前へ `value` を挿入します。`erase` は `position` 番目の要素を削除します。

**制約**

- `insert`: $0 \leq position \leq N$
- `erase`: $0 \leq position < N$

**計算量**

- 期待 `O(log N)`

### set / get

```cpp
void set(int position, const S& value);
S get(int position);
```

`set` は `position` 番目の要素を `value` に置き換えます。`get` は `position` 番目の要素を返します。

**制約**

- $0 \leq position < N$

**計算量**

- 期待 `O(log N)`

### prod

```cpp
S prod(int l, int r);
```

半開区間 `[l,r)` の `op` による積を返します。`l = r` のときは `e()` を返します。

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- 期待 `O(log N)`

### reverse

```cpp
void reverse(int l, int r);
```

半開区間 `[l,r)` の並びを反転します。

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- 期待 `O(log N)`

### apply

```cpp
void apply(int l, int r, F f);
```

半開区間 `[l,r)` の各要素に作用 `f` を遅延適用します。

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- 期待 `O(log N)`

### to_vector

```cpp
vector<S> to_vector();
```

現在の列を `vector` として返します。

**計算量**

- `O(N)`

## 使用例

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/ImplicitTreap.hpp"
using namespace std;

long long op(long long a, long long b) {
    return a + b;
}

long long e() {
    return 0;
}

int main() {
    ImplicitTreap<long long, op, e> treap({1, 2, 3, 4, 5});

    treap.reverse(1, 4);              // {1,4,3,2,5}
    cout << treap.prod(0, 3) << '\n'; // 8

    treap.insert(2, 10);              // {1,4,10,3,2,5}
    treap.erase(3);                   // {1,4,10,2,5}
    treap.set(0, 7);                  // {7,4,10,2,5}
}
```

区間加算・区間和の例です。`S` に区間長を持たせると、lazy propagation で区間全体への加算を扱えます。

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/ImplicitTreap.hpp"
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
    vector<S> a = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}};
    ImplicitTreap<S, op, e, long long, mapping, composition, id> treap(a);

    treap.apply(1, 4, 10);             // {1,12,13,14,5}
    cout << treap.prod(0, 5).sum << '\n'; // 45

    treap.reverse(0, 5);               // {5,14,13,12,1}
    cout << treap.prod(0, 2).sum << '\n'; // 19
}
```

## 注意

- すべての区間は0-indexed半開区間です。
- 乱択平衡二分木なので、各操作の計算量は期待値です。
- 非可換な `op` にも対応するため、反転後の `prod` は反転された順序で計算されます。
- `apply` と `reverse` を併用する場合、`mapping` は反転前後のどちらの区間積にも正しく作用する必要があります。
