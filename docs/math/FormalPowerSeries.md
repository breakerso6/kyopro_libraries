# FormalPowerSeries

`libraries/math/FormalPowerSeries.hpp` は modint などの体上で形式的冪級数を扱うライブラリです。ACL の `atcoder::convolution` を使う高速版を標準にし、`UseNTT=false` で素朴畳み込みにもできます。

## Include

```cpp
#include "libraries/math/FormalPowerSeries.hpp"
```

## 型

```cpp
using mint = atcoder::modint998244353;
using FPS = FormalPowerSeries<mint>;

// NTTを使わない場合
using SlowFPS = FormalPowerSeries<mint, false>;
```

`UseNTT=true` では `atcoder::convolution` が使える `Mint` が必要です。通常は `atcoder::modint998244353` を使います。

## 基本操作

```cpp
FPS f{1, 2, 3};
FPS g(vector<mint>{3, 4});

f.size();
f.empty();
f.resize(n);
f.shrink();
f[i];
f.vec();

f.pre(n);
f.rev();
f.eval(x);
```

- `pre(n)`: $x^n$ 以上を捨てた多項式
- `rev()`: 係数列を反転
- `eval(x)`: $f(x)$

`operator[] const` は範囲外なら `0` を返します。

## 四則演算

```cpp
f + g;
f - g;
f * g;
f + c;
f - c;
f * c;
f / c;

auto [q, r] = f.divmod(g);
f / g;
f % g;
```

多項式除算では `g` が零多項式でない必要があります。

**計算量**

- 加減算: $O(n)$
- 乗算: `UseNTT=true` なら $O(n \log n)$、短い列や `UseNTT=false` では $O(nm)$
- 除算: $O(n \log n)$

## 微分・積分

```cpp
f.diff();
f.integral();
```

`integral()` は $i+1$ の逆元を使います。

**計算量**

- $O(n)$

## 逆元

```cpp
FPS inv = f.inv(n);
```

$f^{-1} \bmod x^n$ を返します。

**制約**

- $0 \leq n$
- $f(0) \neq 0$

**計算量**

- $O(n \log n)$

## log

```cpp
FPS lg = f.log(n);
```

$\log f \bmod x^n$ を返します。

**制約**

- $0 \leq n$
- $f(0) = 1$

**計算量**

- $O(n \log n)$

## exp

```cpp
FPS ex = f.exp(n);
```

$\exp f \bmod x^n$ を返します。

**制約**

- $0 \leq n$
- $f(0) = 0$

**計算量**

- $O(n \log n)$

## pow

```cpp
FPS p = f.pow(k, n);
```

$f^k \bmod x^n$ を返します。

**制約**

- $0 \leq k$
- $0 \leq n$
- 非零定数項の累乗に `Mint::pow(long long)` を使える

**計算量**

- $O(n \log n)$

## 使用例

```cpp
#include <atcoder/modint>
#include "libraries/math/FormalPowerSeries.hpp"
using mint = atcoder::modint998244353;
using FPS = FormalPowerSeries<mint>;

FPS f{1, 2, 3, 4};
FPS inv = f.inv(10);
FPS lg = f.log(10);

FPS g{0, 1, 2};
FPS ex = g.exp(10);
FPS cube = g.pow(3, 10);
```
