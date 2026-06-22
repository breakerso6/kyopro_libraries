# String

## Aho-Corasick

```cpp
#include "libraries/string/AhoCorasick.hpp"
```

`add(pattern)` でパターンを登録し、`build()` 後に `count_matches(text)` で各パターンの出現回数を返します。既定の文字集合は `a` から始まる英小文字26文字です。構築後にパターンを追加しないでください。

## Rolling Hash

```cpp
#include "libraries/string/RollingHash.hpp"
```

法 `2^61-1` のローリングハッシュです。`hash(l,r)` は半開部分文字列、`concat(left,right,right_length)` はハッシュの連結を返します。比較は確率的であり、衝突の可能性はゼロではありません。

## [Manacher](Manacher.md)

```cpp
#include "libraries/string/Manacher.hpp"
Manacher manacher(text);
```

`odd[i]` は中心 `i` の奇数長回文の半径、`even[i]` は境界 `i-1, i` を中心とする偶数長回文の半径です。`is_palindrome(l,r)` で半開部分文字列を `O(1)` 判定できます。構築は `O(N)` です。

詳細は [Manacher.md](Manacher.md) を参照してください。

## [Palindromic Tree](PalindromicTree.md)

```cpp
#include "libraries/string/PalindromicTree.hpp"
PalindromicTree tree(text);
```

異なる非空回文を各ノードで表します。`distinct_count()` は異なる回文数、`palindrome(node)` は対応文字列を返します。`propagate_occurrences()` 後は `nodes[node].occurrences` が全文中の出現回数になります。構築は `O(N log sigma)`、英小文字など固定文字種では実質線形です。出現回数伝播後の文字追加はできません。

詳細は [PalindromicTree.md](PalindromicTree.md) を参照してください。
