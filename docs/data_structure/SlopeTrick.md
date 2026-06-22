# SlopeTrick

`libraries/data_structure/SlopeTrick.hpp` は傾きが単調非減少な凸区分線形関数を管理します。関数全体を保持せず、折れ点を2つのヒープで管理します。

## Include

```cpp
#include "libraries/data_structure/SlopeTrick.hpp"
```

## Template Parameter and Initial State

```cpp
template<class T = long long>
struct SlopeTrick;

SlopeTrick<long long> slope;
```

初期状態はすべての `x` に対して `f(x)=0` です。`T` は順序比較、加減算、整数との乗算相当の計算でオーバーフローしない型を使います。

## Add Operations

```cpp
void add_constant(T c);
void add_a_minus_x(T a);
void add_x_minus_a(T a);
void add_abs(T a);
```

現在の関数へ次を加えます。

| function | added function |
| --- | --- |
| `add_constant(c)` | `c` |
| `add_a_minus_x(a)` | `max(a-x,0)` |
| `add_x_minus_a(a)` | `max(x-a,0)` |
| `add_abs(a)` | `abs(x-a)` |

定数加算は `O(1)`、その他は `O(log N)` です。

## Minimum

```cpp
T min_f() const;
pair<optional<T>, optional<T>> argmin() const;
```

`min_f()` は関数の最小値です。`argmin()` は最小値を取る区間の左端・右端を返します。片側に非有界なら対応する値は `nullopt` です。

計算量: `O(1)`

## Transformations

```cpp
void shift(T delta);
void shift(T left_delta, T right_delta);
void clear_left();
void clear_right();
```

- `shift(delta)`: `g(x)=f(x-delta)`
- `shift(l,r)`: `g(x)=min_{x-r <= y <= x-l} f(y)`。`l <= r` が必要
- `clear_left`: 最小点より左側を水平にする
- `clear_right`: 最小点より右側を水平にする

いずれも `O(1)` です。

## Example

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/SlopeTrick.hpp"
using namespace std;

int main() {
    SlopeTrick<long long> slope;
    for (long long a : {5, -2, 8, 5}) slope.add_abs(a);

    cout << slope.min_f() << '\n'; // 10
    auto [left, right] = slope.argmin();
    cout << *left << ' ' << *right << '\n'; // 5 5

    slope.shift(-2, 3);
    tie(left, right) = slope.argmin();
    cout << *left << ' ' << *right << '\n'; // 3 8
}
```

## Notes

- 凸性を保つ操作だけを提供します。
- `argmin()` の `optional` は、初期状態のように最小点集合が全実数の場合は両方とも `nullopt` です。
- ヒープに追加した折れ点の総数に比例してメモリを使用します。
