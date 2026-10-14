# Rational

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [Rational.hpp](../../libraries/math/Rational.hpp)

`libraries/math/Rational.hpp` は、符号付き整数型を分子・分母に使う既約分数型を提供します。行列や幾何に依存せず単体で利用できます。

## include

```cpp
#include "libraries/math/Rational.hpp"
```

## 共通の制約

- `Int` は対応する符号付き整数型
- 分母は `0` でない。除算の右辺の分子も `0` でない
- 符号反転、積、加減算を含む中間値が演算に用いる整数型に収まる


## 型と基本操作

```cpp
template<class Int = long long>
struct Rational {
    Int numerator;
    Int denominator;

    Rational(Int numerator = 0, Int denominator = 1);
    void normalize();
    long double to_long_double() const;
    bool is_integer() const;
    Int floor() const;
    Int ceil() const;
};

int sign(Rational<Int> value);
Rational<Int> abs(Rational<Int> value);
```

生成時と演算後に約分され、分母は常に正になります。分母に `0` は指定できません。

- 単項 `+`, `-`
- 四則演算 `+`, `-`, `*`, `/` と四則代入
- `==`, `!=`, `<`, `>`, `<=`, `>=`
- `floor()`, `ceil()`, `is_integer()`
- `to_long_double()` による近似値への変換
- `sign(value)`, `abs(value)`

異なる整数型を持つ `Rational` 同士も四則演算・比較でき、結果の整数型には `std::common_type_t` を使います。また、別の整数型を持つ `Rational` から変換構築できます。

## 使用例

```cpp
Rational<long long> a(1, 2);
Rational<long long> b(-2, 3);

auto c = a + b;       // -1/6
auto d = a / b;       // -3/4
assert(c.numerator == -1 && c.denominator == 6);
assert(d.floor() == -1);
assert(d.ceil() == 0);
```

テンプレート引数を省略した `Rational<>` は `Rational<long long>` です。より広い中間値が必要なら `Rational<__int128_t>` も利用できます。

```cpp
using Fraction = Rational<__int128_t>;
Fraction value((__int128_t)1 << 100, 3);
```

## 補足

正規化にはユークリッドの互除法を使うため、各四則演算は整数演算を除いて $O(\log \min(|numerator|, denominator))$ です。加算・乗算・除算では可能な範囲で先に約分してから積を取ります。

固定長整数を使うため、最終結果が収まっていても加減算や大小比較の途中の積が `Int` の範囲を超える場合があります。値域に応じて `long long` と `__int128_t` を使い分けてください。

整数幾何の `geometry_int::Rational128` は `Rational<__int128_t>` の別名です。整数行列の `inverse_matrix_rational` もこの共通型を返します。

## 計算量

- コンストラクタ、`normalize`、四則演算は最大の整数の絶対値を $M$ として $O(\log(M+1))$
- 比較、`floor`、`ceil`、`is_integer`、`to_long_double`、`sign` は $O(1)$
- 整数演算を $O(1)$ として数える
