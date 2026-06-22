# Persistent data structures

すべての更新は新しいroot IDを返し、入力したrootの内容を変更しません。root IDは同じ構造のインスタンス内でのみ使用してください。

## PersistentSegmentTree

```cpp
#include "libraries/data_structure/PersistentSegmentTree.hpp"

PersistentSegmentTree<long long, SumOp> seg(n, 0LL);
int root0 = seg.build(values);
int root1 = seg.set(root0, position, value);
long long sum = seg.prod(root1, left, right);
```

任意モノイドの一点更新・半開区間積です。構築は `O(N)`、更新・取得は `O(log N)`、更新ごとの追加メモリは `O(log N)` です。

## PersistentLazySegmentTree

```cpp
#include "libraries/data_structure/PersistentLazySegmentTree.hpp"

PersistentLazySegmentTree<long long> seg(n);
int root0 = seg.build(values);
int root1 = seg.add(root0, left, right, delta);
long long sum = seg.prod(root1, left, right);
```

区間加算・区間和に特化しています。操作は `O(log N)`、更新ごとの追加メモリは `O(log N)` です。値型 `T` は加算、乗算、ゼロ初期化をサポートする必要があります。

## PersistentDSU

```cpp
#include "libraries/data_structure/PersistentDSU.hpp"

PersistentDSU dsu(n);
int root0 = dsu.build();
int root1 = dsu.merge(root0, u, v);
bool connected = dsu.same(root1, x, y);
```

union by sizeを使い、経路圧縮は行いません。任意の過去rootからmergeしてversionを分岐できます。mergeは `O(log N)` 個の木ノードを追加します。leader探索はDSUの高さと永続配列検索により `O(log^2 N)` です。

## PersistentBinaryTrie

```cpp
#include "libraries/data_structure/PersistentBinaryTrie.hpp"

PersistentBinaryTrie<unsigned, 30> trie;
vector<int> roots{trie.empty_root()};
for (unsigned x : values) roots.push_back(trie.insert(roots.back(), x));
unsigned answer = trie.kth_xor(roots[l], roots[r], query, k);
```

`kth_xor(left_root,right_root,x,k)` はmultiset差 `right-left` に含まれる値と `x` のXORを小さい順に並べた0-indexedの `k` 番目を返します。差分利用時はleft側のmultisetがright側に包含される必要があります。各操作は `O(BITS)` です。
