# Math

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
