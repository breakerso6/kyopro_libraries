# RollingHash

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [RollingHash.hpp](../../libraries/string/RollingHash.hpp)

`libraries/string/RollingHash.hpp` は法 $2^{61}-1$ のローリングハッシュです。部分文字列のハッシュ取得と、ハッシュの連結ができます。

## include

```cpp
#include "libraries/string/RollingHash.hpp"
```

## コンストラクタ

```cpp
RollingHash hash(string_view s);
```

### 計算量

- $O(N)$

## hash

```cpp
RollingHash::ull hash.hash(int l, int r) const;
```

半開部分文字列 `s[l,r)` のハッシュを返します。

### 制約

- $0 \leq l \leq r \leq N$

### 計算量

- $O(1)$

## concat

```cpp
RollingHash::ull RollingHash::concat(ull left, ull right, int right_length);
```

ハッシュ `left` の後ろに、長さ `right_length` の文字列のハッシュ `right` を連結したハッシュを返します。

### 制約

- $0 \leq right\_length$

### 計算量

- 未計算の累乗を $\Delta$ 個追加する場合、$O(\Delta + 1)$
- 必要な累乗が計算済みなら $O(1)$。累乗の追加総数は、要求した最大長に比例する

## 注意

- 比較は確率的です。衝突の可能性はゼロではありません。
- `base` と累乗キャッシュ `powers` は全インスタンスで共有します。`base` を変更する場合は、最初の文字列構築・`concat` より前に行ってください。その後の変更では既存のハッシュとキャッシュが一致しなくなります。
- `concat` に渡す両ハッシュは同じ `base` で計算し、`right_length` は右側文字列の長さと一致させてください。
- 引数なしのコンストラクタは未構築のオブジェクトを作ります。`hash` を呼ぶ前に文字列から構築したオブジェクトを代入してください。

## 使用例

```cpp
#include "libraries/string/RollingHash.hpp"
using namespace std;

int main() {
    RollingHash hash("abcabc");
    assert(hash.hash(0, 3) == hash.hash(3, 6));
    auto joined = RollingHash::concat(hash.hash(0, 1), hash.hash(1, 3), 2);
    assert(joined == hash.hash(0, 3));
}
```
