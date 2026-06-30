# Kitamasa

`libraries/algorithm/Kitamasa.hpp` は線形漸化式の第 `n` 項を高速に求めます。

## Include

```cpp
#include "libraries/algorithm/Kitamasa.hpp"
```

## Functions

```cpp
template<class T>
T kitamasa(long long n, const vector<T>& initial, const vector<T>& c);

template<class T>
vector<T> kitamasa_coefficients(long long n, const vector<T>& c);
```

長さ `K` の漸化式を次の形で渡します。

```text
a_n = c[0] a_{n-K} + c[1] a_{n-K+1} + ... + c[K-1] a_{n-1}
```

`initial[i]` は `a_i` です。

`kitamasa_coefficients(n, c)` は `a_n = sum coef[i] * a_i` となる係数列を返します。

## Complexity

- Time: `O(K^2 log n)`
- Memory: `O(K)`

`T` は `+`、`*`、`+=`、`0`、`1` を扱える型を想定します。modint にも使えます。

## Example

```cpp
#include <bits/stdc++.h>
#include "libraries/algorithm/Kitamasa.hpp"
using namespace std;

int main() {
    vector<long long> initial = {0, 1};
    vector<long long> coef = {1, 1};

    cout << kitamasa<long long>(10, initial, coef) << '\n'; // 55
}
```

## Typical use

- Fibonacci 型の漸化式を `O(log N)` に近い感覚で扱う
- DP の遷移が固定幅線形で、`N` だけ極端に大きい問題
- 行列累乗より定数倍を軽くしたい場合

## Notes

- `initial.size() == c.size()` が必要です。
- `n < K` の場合もそのまま `initial[n]` と同じ値を返します。
- 漸化式の係数順は「古い項から新しい項」です。ここがいちばん間違えやすいです。
