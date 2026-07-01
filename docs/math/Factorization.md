# Factorization

`libraries/math/Factorization.hpp` は符号なし64-bit整数に対する素数判定、素因数分解、約数列挙、約数関数を提供します。素数判定には決定的Miller-Rabin法、分解にはPollard's Rho法を使用します。

## Include

```cpp
#include "libraries/math/Factorization.hpp"
```

すべてのAPIは `factorization` 名前空間にあります。

```cpp
using factorization::u64;
```

Python版は `libraries/math/factorization.py` にあります。

```python
from factorization import is_prime, factorize, divisors, euler_phi
```

## is_prime

```cpp
bool is_prime(u64 n);
```

`n` が素数なら `true` を返します。`0` と `1` は `false` です。

64-bit整数全域で決定的になる基底集合を使用します。計算量の目安は `O(log^3 n)` です。

**制約**

- $0 \leq n < 2^{64}$

## prime_factors

```cpp
vector<u64> prime_factors(u64 n);
```

`n` の素因数を重複込みの昇順で返します。例えば `360` は `{2,2,2,3,3,5}` です。`n >= 1` が必要で、`n=1` なら空配列です。

**制約**

- $1 \leq n < 2^{64}$

## factorize

```cpp
vector<pair<u64, int>> factorize(u64 n);
```

`{素因数, 指数}` を素因数の昇順で返します。`n=1` なら空配列です。

**制約**

- $1 \leq n < 2^{64}$

## divisors

```cpp
vector<u64> divisors(u64 n);
```

正の約数を昇順ですべて返します。`divisors(1)` は `{1}` です。出力サイズを `d(n)` とすると、素因数分解後の列挙は `O(d(n))`、ソートは `O(d(n) log d(n))` です。

**制約**

- $1 \leq n < 2^{64}$

## divisor_count

```cpp
u64 divisor_count(u64 n);
u64 divisor_count_from_factorization(vector<pair<u64, int>> factors);
```

正の約数の個数を返します。

**制約**

- $1 \leq n < 2^{64}$

**計算量**

- `divisor_count`: 素因数分解の計算量
- `divisor_count_from_factorization`: $O(\omega(n))$

## divisor_sum

```cpp
template<class T = u64>
T divisor_sum(u64 n);

template<class T = u64>
T divisor_sum_from_factorization(vector<pair<u64, int>> factors);
```

正の約数の総和を返します。`T` を指定すると ACL の `modint` 系など任意の環で計算できます。

**制約**

- $1 \leq n < 2^{64}$
- `T` は `T(u64)`, `+`, `*` を扱える
- `T = u64` の場合、答えが `u64` に収まる

**計算量**

- `divisor_sum`: 素因数分解の計算量
- `divisor_sum_from_factorization`: $O(\Omega(n))$

## euler_phi

```cpp
u64 euler_phi(u64 n);
u64 euler_phi_from_factorization(u64 n, vector<pair<u64, int>> factors);
```

Eulerのトーシェント関数 $\varphi(n)$ を返します。

**制約**

- $1 \leq n < 2^{64}$

**計算量**

- `euler_phi`: 素因数分解の計算量
- `euler_phi_from_factorization`: $O(\omega(n))$

## mobius

```cpp
int mobius(u64 n);
int mobius_from_factorization(vector<pair<u64, int>> factors);
```

Mobius関数 $\mu(n)$ を返します。平方因子を持つ場合は `0`、相異なる素因数が偶数個なら `1`、奇数個なら `-1` です。

**制約**

- $1 \leq n < 2^{64}$

**計算量**

- `mobius`: 素因数分解の計算量
- `mobius_from_factorization`: $O(\omega(n))$

## is_square_free

```cpp
bool is_square_free(u64 n);
bool is_square_free_from_factorization(vector<pair<u64, int>> factors);
```

`n` が平方因子を持たないなら `true` を返します。

**制約**

- $1 \leq n < 2^{64}$

**計算量**

- `is_square_free`: 素因数分解の計算量
- `is_square_free_from_factorization`: $O(\omega(n))$

## radical

```cpp
u64 radical(u64 n);
u64 radical_from_factorization(vector<pair<u64, int>> factors);
```

`n` を割る相異なる素数の積を返します。

**制約**

- $1 \leq n < 2^{64}$

**計算量**

- `radical`: 素因数分解の計算量
- `radical_from_factorization`: $O(\omega(n))$

## 使用例

```cpp
#include <bits/stdc++.h>
#include <atcoder/modint>
#include "libraries/math/Factorization.hpp"
using namespace std;

int main() {
    using namespace factorization;

    u64 n = 1000000007ULL * 1000000009ULL;
    cout << is_prime(n) << '\n'; // 0

    for (auto [prime, exponent] : factorize(n))
        cout << prime << '^' << exponent << '\n';

    for (u64 divisor : divisors(60)) cout << divisor << ' ';
    cout << '\n'; // 1 2 3 4 5 6 10 12 15 20 30 60

    cout << divisor_count(60) << '\n'; // 12
    cout << divisor_sum(60) << '\n';   // 168
    cout << euler_phi(60) << '\n';     // 16
    cout << mobius(30) << '\n';        // -1

    using mint = atcoder::modint998244353;
    cout << divisor_sum<mint>(60).val() << '\n'; // 168
}
```

## 計算量と乱択性

Pollard's Rho法の実行時間は確率的で、素因数分解の期待計算量の目安は `O(n^(1/4))` です。同じ入力でも内部乱数により実行経路が変わりますが、返される分解結果はソートされるため一定です。

## 注意

- `prime_factors`、`factorize`、約数関数系の入力は `1` 以上です。
- 内部の積は `__uint128_t` で計算し、64-bit乗算のオーバーフローを避けます。
- APIの値型は `uint64_t` です。負数を分解する場合は符号を別に処理してください。
- 極端に多数の約数を持つ数では、`divisors` の出力自体が大きくなります。
