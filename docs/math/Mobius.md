# Mobius

`libraries/math/Mobius.hpp` はMobius関数と、約数・倍数方向のZeta/Mobius変換を提供します。

## Include

```cpp
#include "libraries/math/Mobius.hpp"
```

## mobius_sieve

```cpp
vector<int> mobius_sieve(int n);
```

`mu[0..n]` を返します。`mu[1] = 1` です。

**制約**

- $0 \leq n$

**計算量**

- $O(n)$

## divisor_zeta / divisor_mobius

```cpp
template<class T> vector<T> divisor_zeta(vector<T> f);
template<class T> vector<T> divisor_mobius(vector<T> f);
```

`f` は添字 `1..N` を使います。`f[0]` は未使用です。

`g = divisor_zeta(f)`:

```text
g[n] = sum f[d]  (d | n)
```

`divisor_mobius` はこの逆変換です。

**計算量**

- $O(N \log N)$

## multiple_zeta / multiple_mobius

```cpp

template<class T> vector<T> multiple_zeta(vector<T> f);
template<class T> vector<T> multiple_mobius(vector<T> f);
```

`f` は添字 `1..N` を使います。`f[0]` は未使用です。

`g = multiple_zeta(f)`:

```text
g[d] = sum f[m]  (d | m)
```

`multiple_mobius` はこの逆変換です。

**計算量**

- $O(N \log N)$

## 使用例

```cpp
auto mu = mobius_sieve(10);
// mu[1..10] = 1,-1,-1,0,-1,1,-1,0,0,1

vector<long long> f = {0, 1, 2, 3, 4, 5};
auto g = divisor_zeta(f);
auto restored = divisor_mobius(g);
```

## 典型的な使い方

- `g[n] = sum_{d|n} f[d]` から `f` を復元する
- gcd がちょうど `1` の組を、倍数ごとの個数から数える
- 約数包除を明示的な二重ループより読みやすく書く
