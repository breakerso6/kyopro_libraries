# ModCombinatorics

`libraries/math/ModCombinatorics.hpp` はmodint上の階乗・逆階乗・逆元を前計算し、順列・二項係数・多項係数などを計算します。AtCoder Library の `atcoder::static_modint`、`atcoder::dynamic_modint` に対応します。

## Include

```cpp
#include "libraries/math/ModCombinatorics.hpp"
```

## コンストラクタ

```cpp
ModCombinatorics<Mint> comb;
ModCombinatorics<Mint> comb(int n);
```

`n` を渡した場合、`0..n` まで前計算します。後から大きい `n` が必要になった場合は自動で拡張されます。

**制約**

- `Mint` は `+`, `-`, `*`, `/`, `Mint(int)` を扱える
- `fact[n]` が割れる必要があるため、通常は法が素数で `n < mod`

**計算量**

- 構築: $O(n)$

## ensure

```cpp
void comb.ensure(int n);
```

`0..n` まで前計算します。

**制約**

- $0 \leq n$

**計算量**

- 追加で伸びた長さに比例

## factorial / inv_factorial / inverse

```cpp
Mint comb.factorial(int n);
Mint comb.inv_factorial(int n);
Mint comb.inverse(int n);
```

`n!`、`(n!)^{-1}`、`n^{-1}` を返します。

**制約**

- $0 \leq n$
- `inverse(n)` は $1 \leq n$

**計算量**

- 償却 $O(1)$

## perm / comb / homo

```cpp
Mint comb.perm(int n, int k);
Mint comb.comb(int n, int k);
Mint comb.homo(int n, int k);
```

- `perm(n,k)`: ${}_nP_k$
- `comb(n,k)`: ${}_nC_k$
- `homo(n,k)`: 重複組合せ ${}_{n+k-1}C_k$

範囲外の `k` では `0` を返します。

**計算量**

- 償却 $O(1)$

## multinomial

```cpp
Mint comb.multinomial(vector<int> counts);
Mint comb.multinomial({a, b, c});
Mint comb.multinomial(int n, vector<int> counts);
```

多項係数

$$
\frac{(\sum_i c_i)!}{\prod_i c_i!}
$$

を返します。`n` を渡す overload は `sum(counts) != n` のとき `0` を返します。

**制約**

- `counts[i] >= 0`

**計算量**

- $O(|counts|)$

## catalan

```cpp
Mint comb.catalan(int n);
```

Catalan数

$$
C_n = \frac{1}{n+1}\binom{2n}{n}
$$

を返します。

**制約**

- $0 \leq n$
- `n + 1` が割れる

**計算量**

- 償却 $O(1)$

## derangement

```cpp
Mint comb.derangement(int n);
```

長さ `n` の錯置数を返します。内部で

$$
D_n = (n-1)(D_{n-1}+D_{n-2})
$$

を前計算します。

**制約**

- $0 \leq n$

**計算量**

- 追加で伸びた長さに比例

## 使用例

```cpp
#include <atcoder/modint>
#include "libraries/math/ModCombinatorics.hpp"
using mint = atcoder::modint998244353;

ModCombinatorics<mint> mc(1000000);
mint answer = mc.comb(n, k);
mint ways = mc.multinomial({2, 3, 4});
mint cat = mc.catalan(10);
```

`dynamic_modint` を使う場合は、構築前に `set_mod` を呼んでください。

```cpp
using mint = atcoder::dynamic_modint<0>;
mint::set_mod(1000000007);
ModCombinatorics<mint> mc(1000);
```
