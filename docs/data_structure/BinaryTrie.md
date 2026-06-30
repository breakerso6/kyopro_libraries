# BinaryTrie

`libraries/data_structure/BinaryTrie.hpp` は非負整数のmultisetをビットごとのTrieで管理します。指定値とのxor最小値・最大値を高速に求められます。

## Include

```cpp
#include "libraries/data_structure/BinaryTrie.hpp"
```

## コンストラクタ

```cpp
BinaryTrie<UInt, BITS> trie;
```

`UInt` は符号なし整数型、`BITS` は見るビット数です。

**制約**

- `UInt` は符号なし整数型
- $1 \leq BITS \leq$ `numeric_limits<UInt>::digits`

## size / count

```cpp
int trie.size() const;
int trie.count(UInt x) const;
```

全要素数と、値 `x` の個数を返します。

**計算量**

- `size`: $O(1)$
- `count`: $O(BITS)$

## insert / erase

```cpp
void trie.insert(UInt x);
bool trie.erase(UInt x);
```

`insert` は値を1個追加します。`erase` は値を1個削除し、削除できたなら `true` を返します。

**計算量**

- $O(BITS)$

## min_xor / max_xor

```cpp
UInt trie.min_xor(UInt x) const;
UInt trie.max_xor(UInt x) const;
```

保存された値 `v` について、`v xor x` の最小値・最大値を返します。

**制約**

- `trie.size() > 0`

**計算量**

- $O(BITS)$

## 使用例

```cpp
BinaryTrie<unsigned, 30> trie;
trie.insert(5);
trie.insert(10);

cout << trie.min_xor(7) << '\n';
cout << trie.max_xor(7) << '\n';
```
