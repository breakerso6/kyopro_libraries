# Factorization

`libraries/math/Factorization.hpp` は符号なし64-bit整数に対する素数判定、素因数分解、約数列挙を提供します。素数判定には決定的Miller-Rabin法、分解にはPollard's Rho法を使用します。

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
from factorization import is_prime, factorize, divisors
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

## 使用例

```cpp
#include <bits/stdc++.h>
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
}
```

## 計算量と乱択性

Pollard's Rho法の実行時間は確率的で、素因数分解の期待計算量の目安は `O(n^(1/4))` です。同じ入力でも内部乱数により実行経路が変わりますが、返される分解結果はソートされるため一定です。

## 注意

- `prime_factors`、`factorize`、`divisors` の入力は `1` 以上です。
- 内部の積は `__uint128_t` で計算し、64-bit乗算のオーバーフローを避けます。
- APIの値型は `uint64_t` です。負数を分解する場合は符号を別に処理してください。
- 極端に多数の約数を持つ数では、`divisors` の出力自体が大きくなります。
