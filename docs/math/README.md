# Math

数論、線形代数、組合せカウント系のライブラリです。

## 一覧

| library | purpose |
| --- | --- |
| [XorBasis.hpp](XorBasis.md) | XOR線形基底 |
| [IntegerMath.hpp](IntegerMath.md) | 整数除算、拡張GCD、CRT、floor_sum |
| [ModCombinatorics.hpp](ModCombinatorics.md) | ACL modint対応の階乗・二項係数・多項係数 |
| [Factorization.hpp](Factorization.md) | 64-bit整数の素数判定・素因数分解・約数関数 |
| [Matrix.hpp](Matrix.md) | 行列積、累乗、行列式、逆行列 |
| [Mobius.hpp](Mobius.md) | Mobius関数、約数/倍数Zeta変換 |
| [MappingTwelvefold.hpp](MappingTwelvefold.md) | 写像12相の基本カウント |

## [XorBasis](XorBasis.md)

```cpp
#include "libraries/math/XorBasis.hpp"
```

整数列のXOR線形基底を管理します。

- `insert(x)`: rankが増えた場合に `true`
- `contains(x)`: 挿入値の部分集合XORで表現可能か
- `max_xor(seed)`: `seed` から得られる最大XOR
- `rank`: 基底のrank

各操作は `O(BITS)`、メモリは `O(BITS)` です。

詳細は [XorBasis.md](XorBasis.md) を参照してください。

## [IntegerMath](IntegerMath.md)

```cpp
#include "libraries/math/IntegerMath.hpp"
```

`safe_mod`、`floor_div`、`ceil_div`、`ext_gcd`、`pow_mod`、`inv_mod`、`crt`、`floor_sum` を提供します。

詳細は [IntegerMath.md](IntegerMath.md) を参照してください。

## [ModCombinatorics](ModCombinatorics.md)

```cpp
#include "libraries/math/ModCombinatorics.hpp"
```

ACLの `atcoder::modint` 系でも使える階乗・逆階乗・逆元テーブルです。順列、二項係数、多項係数、Catalan数、錯置数を計算できます。

詳細は [ModCombinatorics.md](ModCombinatorics.md) を参照してください。

## [64-bit factorization](Factorization.md)

```cpp
#include "libraries/math/Factorization.hpp"
using namespace factorization;
```

- `is_prime(n)`: 決定的Miller-Rabin素数判定
- `prime_factors(n)`: 重複を含む昇順の素因数列
- `factorize(n)`: `{素因数, 指数}` の列
- `divisors(n)`: 昇順の正の約数列
- `divisor_count(n)`, `divisor_sum<T>(n)`: 約数個数・約数和
- `euler_phi(n)`, `mobius(n)`, `is_square_free(n)`, `radical(n)`: 代表的な乗法的関数

Pollard's Rho法により `1` 以上の符号なし64-bit整数を扱います。`prime_factors(1)` と `factorize(1)` は空、`divisors(1)` は `{1}` です。`divisor_sum<T>` はACLの `modint` 系にも対応します。実行時間は確率的で、素因数分解の目安は `O(n^(1/4))` です。

詳細は [Factorization.md](Factorization.md) を参照してください。

Python版は `libraries/math/factorization.py` にあり、同じく素因数分解と約数関数を提供します。

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
