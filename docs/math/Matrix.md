# Matrix

`libraries/math/Matrix.hpp` は小から中規模の行列計算用のユーティリティです。

## Include

```cpp
#include "libraries/math/Matrix.hpp"
```

## Types

```cpp
template<class T>
using Matrix = vector<vector<T>>;

template<class Int = long long>
struct Rational;
```

`Rational<Int>` は整数行列の逆行列を有理数として返すための簡易分数型です。

## Functions

```cpp
Matrix<T> matrix_identity<T>(int n);
Matrix<T> matrix_mul(const Matrix<T>& a, const Matrix<T>& b);
Matrix<T> matrix_pow(Matrix<T> a, long long e);
T determinant(Matrix<T> a);
optional<Matrix<T>> inverse_matrix(Matrix<T> a);
Int determinant_bareiss(Matrix<Int> a);
optional<Matrix<Rational<Int>>> inverse_matrix_rational(const Matrix<Int>& a);
```

## Complexity

`N x N` 行列について:

- `matrix_mul`: `O(N^3)`
- `matrix_pow`: `O(N^3 log e)`
- `determinant`: `O(N^3)`
- `inverse_matrix`: `O(N^3)`
- `determinant_bareiss`: `O(N^3)`

## Example: matrix power over F_p

```cpp
using mint = atcoder::modint998244353;

Matrix<mint> a = {
    {1, 1},
    {1, 0},
};
auto b = matrix_pow(a, 10);
cout << b[0][1].val() << '\n'; // Fibonacci(10)
```

## Example: integer determinant and inverse

```cpp
Matrix<long long> a = {
    {1, 2},
    {3, 4},
};

cout << determinant_bareiss(a) << '\n'; // -2

auto inv = inverse_matrix_rational(a);
// inv[0][0] = -2, inv[0][1] = 1
// inv[1][0] = 3/2, inv[1][1] = -1/2
```

## Notes

- `determinant` と `inverse_matrix` は割り算できる体上の型向けです。
- 整数の行列式には `determinant_bareiss` を使うと、途中の分数を避けられます。
- `inverse_matrix_rational` は逆行列が存在しない場合 `nullopt` を返します。
- `Rational<long long>` は分子分母の積が大きいとオーバーフローします。
