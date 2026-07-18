# ImplicitTreapBeats

`libraries/data_structure/ImplicitTreapBeats.hpp` は列を乱択平衡二分木として管理し、位置指定の挿入・削除・区間反転に加えて、Segment Tree Beats 型の区間 `chmin`、区間 `chmax`、区間加算、区間集約を行うデータ構造です。安定したノードハンドルから現在位置を取得し、同じハンドルを保ったまま要素を移動することもできます。

## Include

```cpp
#include "libraries/data_structure/ImplicitTreapBeats.hpp"
```

## コンストラクタ

```cpp
ImplicitTreapBeats treap;
ImplicitTreapBeats treap(values);
```

`values` は `vector<long long>` です。

**制約**

- 和と更新結果が `long long` に収まる

**計算量**

- 空構築: `O(1)`
- `values` から構築: `O(N)`

## ノードハンドル

```cpp
using Handle = int;
```

`Handle` はTreap内の1要素を識別する安定したIDです。挿入・削除・反転・各種区間更新によって位置や値が変わっても、アクティブな要素のハンドルは変わりません。

- `insert` が新しい要素のハンドルを返す
- 初期構築した要素のハンドルは `handle_at` または `to_handles` で取得する
- `extract` と `reinsert` で同じハンドルを保ったまま要素を移動できる
- `build` を再度呼ぶと、それ以前のすべてのハンドルは無効になる

## 列操作

```cpp
int size() const;
bool empty() const;
Handle insert(int position, long long value);
void erase(int position);
void set(int position, long long value);
long long get(int position);
void reverse(int l, int r);
vector<long long> to_vector();
vector<Handle> to_handles();
```

`insert` は `position` 番目の直前へ値を挿入し、新しい要素のハンドルを返します。戻り値は不要なら無視できます。`erase` は `position` 番目の要素を削除します。`set`、`get` は一点更新・一点取得です。`reverse` は半開区間 `[l,r)` の並びを反転します。`to_handles` は現在の並び順に対応するハンドル列を返します。

**制約**

- `insert`: $0 \leq position \leq N$
- `erase`, `set`, `get`: $0 \leq position < N$
- `reverse`: $0 \leq l \leq r \leq N$

**計算量**

- `to_vector`: `O(N)`
- `to_handles`: `O(N)`
- それ以外: 期待 `O(log N)`

## ハンドル操作

```cpp
Handle handle_at(int position);
int index_of(Handle handle);
Handle extract(int position);
void reinsert(int position, Handle handle);
void reinsert(int position, Handle handle, long long value);
```

- `handle_at` は現在 `position` 番目にある要素のハンドルを返す
- `index_of` はアクティブな `handle` の現在位置を返す。反転が遅延中でも論理的な位置を取得できる
- `extract` は要素を列から取り出してハンドルを返す
- 2引数の `reinsert` は取り出したときの値で再挿入する
- 3引数の `reinsert` は値を `value` に置き換えて再挿入する

**制約**

- `handle_at`, `extract`: $0 \leq position < N$
- `index_of`: `handle` は現在のTreapに含まれるアクティブなハンドル
- `reinsert`: $0 \leq position \leq N$。`handle` はこのTreapから `extract` され、まだ再挿入されていないもの

**計算量**

- 期待 `O(log N)`

## 更新関数

```cpp
void range_chmin(int l, int r, long long x);
void range_chmax(int l, int r, long long x);
void range_add(int l, int r, long long x);
```

半開区間 `[l,r)` の各値 `a[i]` を次のように更新します。

| function | update |
| --- | --- |
| `range_chmin` | `a[i] = min(a[i], x)` |
| `range_chmax` | `a[i] = max(a[i], x)` |
| `range_add` | `a[i] += x` |

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- 期待ならし `O(log^2 N)`

## クエリ関数

```cpp
long long range_sum(int l, int r);
long long range_min(int l, int r);
long long range_max(int l, int r);
```

`range_sum`、`range_min`、`range_max` は半開区間 `[l,r)` の和、最小値、最大値を返します。

**制約**

- `range_sum`: $0 \leq l \leq r \leq N$
- `range_min`, `range_max`: $0 \leq l < r \leq N$

**計算量**

- 期待 `O(log N)`

## 使用例

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/ImplicitTreapBeats.hpp"
using namespace std;

int main() {
    ImplicitTreapBeats treap(vector<long long>{5, 1, 8, 6});
    auto handles = treap.to_handles();

    treap.range_chmin(0, 4, 6); // {5,1,6,6}
    treap.range_add(1, 3, 2);   // {5,3,8,6}
    treap.range_chmax(0, 2, 4); // {5,4,8,6}
    treap.reverse(1, 4);        // {5,6,8,4}
    cout << treap.index_of(handles[1]) << '\n'; // 元の値1は現在 index 3
    treap.insert(2, 10);        // {5,6,10,8,4}

    cout << treap.range_sum(0, 5) << '\n'; // 33
    cout << treap.range_min(1, 4) << '\n'; // 6
    cout << treap.range_max(0, 3) << '\n'; // 10
}
```

## 注意

- すべての区間は0-indexed半開区間です。
- `range_sum` は空区間に対して `0` を返します。空区間への `range_min`、`range_max` は利用しないでください。
- 内部の和と更新値は `long long` です。積 `x * 区間長` や累積和が範囲内に収まる必要があります。
- 乱択平衡二分木なので、計算量は期待値です。
- 一般の遅延セグメント木ではなく、各部分木が持つ第1・第2最小値・最大値を使って更新を枝刈りします。
- ハンドルは同じTreapオブジェクト内でのみ有効です。`build` による再構築後や別のTreapへ持ち越してはいけません。
- `erase` した要素、および `extract` 後でまだ再挿入していない要素に対して `index_of` を呼んではいけません。
