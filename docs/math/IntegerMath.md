# IntegerMath

`libraries/math/IntegerMath.hpp` は整数演算の基本関数を `integer_math` 名前空間で提供します。ACLの `math` に近い `pow_mod`、`inv_mod`、`crt`、`floor_sum` も含みます。

## Include

```cpp
#include "libraries/math/IntegerMath.hpp"
```

## safe_mod

```cpp
T safe_mod(T x, T m);
```

`x mod m` を `[0,m)` の値で返します。

**制約**

- `m > 0`

**計算量**

- $O(1)$

## floor_div / ceil_div

```cpp
T floor_div(T a, T b);
T ceil_div(T a, T b);
```

それぞれ $\lfloor a/b \rfloor$、$\lceil a/b \rceil$ を返します。負数にも対応します。

**制約**

- `b != 0`

**計算量**

- $O(1)$

## gcd / lcm

```cpp
T gcd(T a, T b);
T lcm(T a, T b);
```

最大公約数・最小公倍数を返します。

**計算量**

- $O(\log \min(|a|, |b|))$

## ext_gcd

```cpp
ExtGCDResult ext_gcd(long long a, long long b);
```

`gcd`, `x`, `y` を持つ構造体を返します。`a*x + b*y = gcd` を満たします。

```cpp
struct ExtGCDResult {
    long long gcd, x, y;
};
```

## pow_mod

```cpp
long long pow_mod(long long x, long long n, long long m);
```

$x^n \bmod m$ を返します。

**制約**

- $0 \leq n$
- $1 \leq m$

**計算量**

- $O(\log n)$

## inv_mod

```cpp
long long inv_mod(long long x, long long m);
```

$xy \equiv 1 \pmod m$ となる `y` を `[0,m)` で返します。

**制約**

- $\gcd(x,m)=1$
- $1 \leq m$

**計算量**

- $O(\log m)$

## crt

```cpp
pair<long long, long long> crt(vector<long long> r, vector<long long> m);
```

連立合同式 `x = r[i] (mod m[i])` を解きます。解がある場合は `{y, z}` を返し、`x = y (mod z)` で表します。解がない場合は `{0, 0}` を返します。

**制約**

- `r.size() == m.size()`
- $1 \leq m[i]$

**計算量**

- $O(N \log \mathrm{lcm}(m[i]))$

## floor_sum

```cpp
long long floor_sum(long long n, long long m, long long a, long long b);
```

$$
\sum_{i=0}^{n-1} \left\lfloor \frac{a i + b}{m} \right\rfloor
$$

を返します。`a`, `b` は負でも構いません。

**制約**

- $0 \leq n$
- $1 \leq m$
- 答えが `long long` に収まる

**計算量**

- $O(\log m + \log |a|)$

## 使用例

```cpp
using namespace integer_math;

auto [x, mod] = crt({2, 3}, {3, 5});
cout << x << ' ' << mod << '\n'; // 8 15

cout << floor_sum(4, 10, 6, 3) << '\n';
```
