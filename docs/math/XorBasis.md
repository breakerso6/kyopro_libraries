# XorBasis

`libraries/math/XorBasis.hpp` は挿入された整数集合の部分集合xorで作れる値を管理します。

## Include

```cpp
#include "libraries/math/XorBasis.hpp"
```

## コンストラクタ

```cpp
XorBasis<UInt, BITS> basis;
```

`UInt` は符号なし整数型、`BITS` は見るビット数です。

**制約**

- `UInt` はxor、比較、シフトを扱える整数型

## insert

```cpp
bool basis.insert(UInt x);
```

`x` を基底に追加します。rankが増えた場合は `true`、既存の基底で表現できる場合は `false` です。

**計算量**

- $O(BITS)$

## contains

```cpp
bool basis.contains(UInt x) const;
```

挿入済みの値の部分集合xorで `x` を表現できるかを返します。

**計算量**

- $O(BITS)$

## max_xor

```cpp
UInt basis.max_xor(UInt seed = 0) const;
```

`seed` に表現可能なxor値をxorして得られる最大値を返します。

**計算量**

- $O(BITS)$

## メンバ変数

| メンバ | 説明 |
| --- | --- |
| `rank` | 線形基底のrank |
| `basis[b]` | 最高bitが `b` の基底 |

## 使用例

```cpp
XorBasis<unsigned, 8> basis;
basis.insert(3);
basis.insert(5);

cout << basis.contains(6) << '\n'; // 1
cout << basis.max_xor() << '\n';
```
