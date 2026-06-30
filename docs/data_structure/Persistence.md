# Persistent data structures

永続データ構造は、更新前の状態を壊さずに新しい `root` を返します。各 `root` は同じインスタンス内でだけ有効です。

このページでは以下をまとめます。

- `PersistentSegmentTree.hpp`
- `PersistentLazySegmentTree.hpp`
- `PersistentDSU.hpp`
- `PersistentBinaryTrie.hpp`

## 共通の注意

- 更新関数は新しい `root` ID を返します。
- 古い `root` を渡した操作は、その時点の状態に対して実行されます。
- `root` ID は別インスタンスへ渡せません。
- 内部ノードは `vector` に蓄積されます。不要になったversionを個別に解放する機能はありません。

## PersistentSegmentTree

任意モノイドの一点更新・半開区間積を永続化したセグメント木です。

### Include

```cpp
#include "libraries/data_structure/PersistentSegmentTree.hpp"
```

### コンストラクタ

```cpp
PersistentSegmentTree<S, Op> seg(int n, S identity, Op op = Op());
```

**制約**

- $1 \leq n$
- `Op` は結合的
- `identity` は `Op` の単位元

**計算量**

- $O(1)$

### build

```cpp
int root = seg.build();
int root = seg.build(values);
```

初期versionを作って、その `root` を返します。`values` を省略した場合は全要素が `identity` です。

**制約**

- `values` を渡す場合は `values.size() == n`

**計算量**

- $O(n)$

### set

```cpp
int new_root = seg.set(root, position, value);
```

`root` の配列に対して `a[position] = value` とした新しいversionを返します。`root` の内容は変わりません。

**制約**

- `root` は `seg` が返したroot
- $0 \leq position < n$

**計算量**

- $O(\log n)$

### get / prod

```cpp
S x = seg.get(root, position);
S x = seg.prod(root, l, r);
```

`get` は1点の値、`prod` は `op(a[l], ..., a[r-1])` を返します。`l = r` のときは `identity` を返します。

**制約**

- `root` は `seg` が返したroot
- `get`: $0 \leq position < n$
- `prod`: $0 \leq l \leq r \leq n$

**計算量**

- $O(\log n)$

### 使用例

```cpp
struct Sum {
    long long operator()(long long a, long long b) const { return a + b; }
};

PersistentSegmentTree<long long, Sum> seg(5, 0);
int root0 = seg.build(vector<long long>{1, 2, 3, 4, 5});
int root1 = seg.set(root0, 2, 10);

cout << seg.prod(root0, 0, 5) << '\n'; // 15
cout << seg.prod(root1, 0, 5) << '\n'; // 22
```

## PersistentLazySegmentTree

区間加算・区間和に特化した永続遅延セグメント木です。

### Include

```cpp
#include "libraries/data_structure/PersistentLazySegmentTree.hpp"
```

### コンストラクタ

```cpp
PersistentLazySegmentTree<T> seg(int n);
```

**制約**

- $1 \leq n$
- `T` は加減算と整数倍を扱える

**計算量**

- $O(1)$

### build

```cpp
int root = seg.build();
int root = seg.build(values);
```

初期versionを作ります。`values` を省略した場合は全要素が `T{}` です。

**制約**

- `values` を渡す場合は `values.size() == n`

**計算量**

- $O(n)$

### add

```cpp
int new_root = seg.add(root, l, r, value);
```

半開区間 `[l,r)` に `value` を加算した新しいversionを返します。

**制約**

- `root` は `seg` が返したroot
- $0 \leq l \leq r \leq n$

**計算量**

- $O(\log n)$

### get / prod

```cpp
T x = seg.get(root, position);
T x = seg.prod(root, l, r);
```

`get` は1点の値、`prod` は区間和を返します。

**制約**

- `root` は `seg` が返したroot
- `get`: $0 \leq position < n$
- `prod`: $0 \leq l \leq r \leq n$

**計算量**

- $O(\log n)$

## PersistentDSU

Union-Findを永続化した構造です。union by size を使い、経路圧縮は行いません。

### Include

```cpp
#include "libraries/data_structure/PersistentDSU.hpp"
```

### コンストラクタ

```cpp
PersistentDSU dsu(int n);
```

**制約**

- $1 \leq n$

### build

```cpp
int root = dsu.build();
```

$n$ 頂点 $0$ 辺の状態を作ります。

**計算量**

- $O(n)$

### merge

```cpp
int new_root = dsu.merge(root, a, b);
```

`root` の状態に辺 `(a,b)` を追加した新しいversionを返します。すでに連結なら同じ `root` を返します。

**制約**

- `root` は `dsu` が返したroot
- $0 \leq a,b < n$

**計算量**

- $O(\log^2 n)$

### leader / same / size

```cpp
int leader = dsu.leader(root, v);
bool ok = dsu.same(root, a, b);
int sz = dsu.size(root, v);
```

指定versionでの代表元、連結判定、連結成分サイズを返します。

**制約**

- `root` は `dsu` が返したroot
- $0 \leq v,a,b < n$

**計算量**

- $O(\log^2 n)$

## PersistentBinaryTrie

非負整数のmultisetを永続化したBinary Trieです。prefix versionの差を取ることで、区間内の値に対するXOR最小/最大やk番目を処理できます。

### Include

```cpp
#include "libraries/data_structure/PersistentBinaryTrie.hpp"
```

### コンストラクタ

```cpp
PersistentBinaryTrie<UInt, BITS> trie;
```

`UInt` は符号なし整数型、`BITS` は見るビット数です。

### empty_root / size / count

```cpp
int root = trie.empty_root();
int s = trie.size(root);
int c = trie.count(root, value);
```

空のroot、multisetのサイズ、値の個数を返します。

**計算量**

- `empty_root`, `size`: $O(1)$
- `count`: $O(BITS)$

### insert / erase

```cpp
int new_root = trie.insert(root, value);
int new_root = trie.erase(root, value);
```

値を1個追加/削除した新しいversionを返します。

**制約**

- `erase` は `count(root, value) > 0`

**計算量**

- $O(BITS)$

### kth_xor / min_xor / max_xor

```cpp
UInt x = trie.kth_xor(left_root, right_root, query, k);
UInt x = trie.min_xor(left_root, right_root, query);
UInt x = trie.max_xor(left_root, right_root, query);
```

multiset差 `right_root - left_root` に含まれる値 `v` について、`v xor query` を昇順に並べたときの値を返します。

**制約**

- `left_root` のmultisetは `right_root` に包含される
- `kth_xor`: $0 \leq k < size(right)-size(left)$
- `min_xor`, `max_xor`: 差集合が空でない

**計算量**

- $O(BITS)$

### 使用例

```cpp
PersistentBinaryTrie<unsigned, 30> trie;
vector<int> roots{trie.empty_root()};
for (unsigned x : values) roots.push_back(trie.insert(roots.back(), x));

// values[l..r) 内で x との xor が最小になる値の xor 値
unsigned best = trie.min_xor(roots[l], roots[r], x);
```
