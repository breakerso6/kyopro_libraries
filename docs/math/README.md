# Math

数論、線形代数、組合せカウント系のライブラリです。

## Quick reference

| library | purpose |
| --- | --- |
| `XorBasis.hpp` | XOR線形基底 |
| [Factorization.hpp](Factorization.md) | 64-bit整数の素数判定・素因数分解 |
| [Matrix.hpp](Matrix.md) | 行列積、累乗、行列式、逆行列 |
| [Mobius.hpp](Mobius.md) | Mobius関数、約数/倍数Zeta変換 |
| [MappingTwelvefold.hpp](MappingTwelvefold.md) | 写像12相の基本カウント |

## XorBasis

```cpp
#include "libraries/math/XorBasis.hpp"
```

整数列のXOR線形基底を管理します。

- `insert(x)`: rankが増えた場合に `true`
- `contains(x)`: 挿入値の部分集合XORで表現可能か
- `max_xor(seed)`: `seed` から得られる最大XOR
- `rank`: 基底のrank

各操作は `O(BITS)`、メモリは `O(BITS)` です。

## [64-bit factorization](Factorization.md)

```cpp
#include "libraries/math/Factorization.hpp"
using namespace factorization;
```

- `is_prime(n)`: 決定的Miller-Rabin素数判定
- `prime_factors(n)`: 重複を含む昇順の素因数列
- `factorize(n)`: `{素因数, 指数}` の列
- `divisors(n)`: 昇順の正の約数列

Pollard's Rho法により `1` 以上の符号なし64-bit整数を扱います。`prime_factors(1)` と `factorize(1)` は空、`divisors(1)` は `{1}` です。実行時間は確率的で、素因数分解の目安は `O(n^(1/4))` です。

詳細は [Factorization.md](Factorization.md) を参照してください。

Python版は `libraries/math/factorization.py` にあり、同じく `is_prime`、`prime_factors`、`factorize`、`divisors` を提供します。

## [Matrix](Matrix.md)

```cpp
#include "libraries/math/Matrix.hpp"
```

`matrix_mul`、`matrix_pow`、`determinant`、`inverse_matrix` を提供します。`determinant` と `inverse_matrix` は `operator/` を持つ体上の型向けです。整数行列の行列式はBareiss法の `determinant_bareiss`、整数行列の有理数逆行列は `inverse_matrix_rational` を使います。

詳細は [Matrix.md](Matrix.md) を参照してください。

## [Mobius](Mobius.md)

```cpp
#include "libraries/math/Mobius.hpp"
```

`mobius_sieve(n)` でMobius関数を作ります。約数方向の `divisor_zeta/divisor_mobius`、倍数方向の `multiple_zeta/multiple_mobius` もあります。

詳細は [Mobius.md](Mobius.md) を参照してください。

## [Mapping twelvefold](MappingTwelvefold.md)

```cpp
#include "libraries/math/MappingTwelvefold.hpp"
```

写像12相の基本個数を `MappingTwelvefold<T>` で前計算します。区別あり/なし、単射/全射/制約なしの代表式をメソッドとして持ちます。

詳細は [MappingTwelvefold.md](MappingTwelvefold.md) を参照してください。
