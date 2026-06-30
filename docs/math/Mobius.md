# Mobius

`libraries/math/Mobius.hpp` はMobius関数と、約数・倍数方向のZeta/Mobius変換を提供します。

## Include

```cpp
#include "libraries/math/Mobius.hpp"
```

## Functions

```cpp
vector<int> mobius_sieve(int n);

template<class T> vector<T> divisor_zeta(vector<T> f);
template<class T> vector<T> divisor_mobius(vector<T> f);
template<class T> vector<T> multiple_zeta(vector<T> f);
template<class T> vector<T> multiple_mobius(vector<T> f);
```

`f` は添字 `1..N` を使います。`f[0]` は未使用です。

## Meaning

`g = divisor_zeta(f)`:

```text
g[n] = sum f[d]  (d | n)
```

`g = multiple_zeta(f)`:

```text
g[d] = sum f[m]  (d | m)
```

`divisor_mobius` と `multiple_mobius` はそれぞれ逆変換です。

## Complexity

- `mobius_sieve`: `O(N)`
- each transform: `O(N log N)`

## Example

```cpp
auto mu = mobius_sieve(10);
// mu[1..10] = 1,-1,-1,0,-1,1,-1,0,0,1

vector<long long> f = {0, 1, 2, 3, 4, 5};
auto g = divisor_zeta(f);
auto restored = divisor_mobius(g);
```

## Typical use

- `g[n] = sum_{d|n} f[d]` から `f` を復元する
- gcd がちょうど `1` の組を、倍数ごとの個数から数える
- 約数包除を明示的な二重ループより読みやすく書く
