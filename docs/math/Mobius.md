# Mobius

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [Mobius.hpp](../../libraries/math/Mobius.hpp)

`libraries/math/Mobius.hpp` はMobius関数と、約数・倍数方向のZeta/Mobius変換を提供します。

## include

```cpp
#include "libraries/math/Mobius.hpp"
```

## mobius_sieve

```cpp
vector<int> mobius_sieve(int n);
```

`mu[0..n]` を返します。`mu[1] = 1` です。

### 制約

- $1 \leq n$

### 計算量

- $O(n)$

## divisor_zeta / divisor_mobius

```cpp
template<class T> vector<T> divisor_zeta(vector<T> f);
template<class T> vector<T> divisor_mobius(vector<T> f);
```

`f` は添字 `1..N` を使います。`f[0]` は未使用です。

`divisor_zeta` は `d = 1, ..., N` の順に、各真の倍数 `m` へ `f[m] += f[d]` を行います。参照する `f[d]` は更新済みの値です。`divisor_mobius` は `d` を降順に走査して減算し、この逐次操作を取り消します。

### 制約

- `T` は `+=` と `-=` に対応する
- 復元には、加減算が互いに逆操作となり、中間値が型の範囲に収まる必要がある

### 計算量

- $O(N \log N)$

## multiple_zeta / multiple_mobius

```cpp

template<class T> vector<T> multiple_zeta(vector<T> f);
template<class T> vector<T> multiple_mobius(vector<T> f);
```

`f` は添字 `1..N` を使います。`f[0]` は未使用です。

`multiple_zeta` は `d = N, ..., 1` の順に、各真の倍数 `m` について `f[d] += f[m]` を行います。参照する `f[m]` は更新済みの値です。`multiple_mobius` は `d` を昇順に走査して減算し、この逐次操作を取り消します。

### 制約

- `T` の条件は約数方向の操作と同じ

### 計算量

- $O(N \log N)$

## 使用例

```cpp
auto mu = mobius_sieve(10);
// mu[1..10] = 1,-1,-1,0,-1,1,-1,0,0,1

vector<long long> f = {0, 1, 2, 3, 4, 5};
auto g = divisor_zeta(f);
auto restored = divisor_mobius(g);
```

## 現在の変換の意味

現在の実装は、入力配列に対する通常の約数和・倍数和とは異なります。例えば `divisor_zeta(vector<int>{0, 1, 0, 0, 0})` の添字 `4` は `2` になりますが、入力の約数和は `1` です。更新済みの値を再利用するため、同じ入力値が複数回加算されます。

`divisor_mobius(divisor_zeta(f))` と `multiple_mobius(multiple_zeta(f))` は上記の逐次操作の復元に使えます。通常の約数和・倍数和や、それに対する Möbius 反転として利用するには実装の修正が必要です。
