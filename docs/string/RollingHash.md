# RollingHash

`libraries/string/RollingHash.hpp` は法 $2^{61}-1$ のローリングハッシュです。部分文字列のハッシュ取得と、ハッシュの連結ができます。

## Include

```cpp
#include "libraries/string/RollingHash.hpp"
```

## コンストラクタ

```cpp
RollingHash hash(string_view s);
```

**計算量**

- $O(N)$

## hash

```cpp
RollingHash::ull x = hash.hash(int l, int r) const;
```

半開部分文字列 `s[l,r)` のハッシュを返します。

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- $O(1)$

## concat

```cpp
RollingHash::ull h = RollingHash::concat(left, right, right_length);
```

ハッシュ `left` の後ろに、長さ `right_length` の文字列のハッシュ `right` を連結したハッシュを返します。

**制約**

- $0 \leq right\_length$

**計算量**

- 償却 $O(1)$

## 注意

- 比較は確率的です。衝突の可能性はゼロではありません。
- `base` は静的メンバです。変更する場合はハッシュ構築前に行ってください。
