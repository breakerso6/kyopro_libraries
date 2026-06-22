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
