# WaveletMatrix

`libraries/data_structure/WaveletMatrix.hpp` は静的配列に対する順序統計量と値域頻度を扱うWavelet Matrixです。値を内部で座標圧縮するため、負数や大きな整数にも利用できます。

## Include

```cpp
#include "libraries/data_structure/WaveletMatrix.hpp"
```

## テンプレート引数

```cpp
template<class T>
struct WaveletMatrix;
```

`T` は比較とコピーが可能な値型です。

## コンストラクタ

```cpp
WaveletMatrix<T> wm(values);
```

`values` を保持する静的構造を構築します。構築後の値変更には対応しません。

**制約**

- `T` は比較可能

**計算量**

- 構築: $O(N \log \sigma)$
- メモリ: $O(N \log \sigma)$

`sigma` は異なる値の個数です。

## 関数

### size

```cpp
int size() const;
```

構築時の配列長を返します。

**計算量**

- $O(1)$

### kth_smallest / kth_largest

```cpp
T kth_smallest(int l, int r, int k) const;
T kth_largest(int l, int r, int k) const;
```

半開区間 `[l,r)` の値を昇順または降順に並べたときの、0-indexedで `k` 番目の値を返します。`0 <= k < r-l` が必要です。

**制約**

- $0 \leq l < r \leq N$
- $0 \leq k < r-l$

**計算量**

- `O(log sigma)`

### count_less / range_freq / count

```cpp
int count_less(int l, int r, const T& upper) const;
int range_freq(int l, int r, const T& lower, const T& upper) const;
int count(int l, int r, const T& value) const;
```

- `count_less`: `value < upper` を満たす要素数
- `range_freq`: `lower <= value < upper` を満たす要素数
- `count`: `value` と等しい要素数

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- `O(log sigma)`

### prev_value / next_value

```cpp
optional<T> prev_value(int l, int r, const T& upper) const;
optional<T> next_value(int l, int r, const T& lower) const;
```

`prev_value` は `upper` 未満の最大値、`next_value` は `lower` 以上の最小値を返します。条件を満たす値がなければ `nullopt` です。

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- `O(log sigma)`

## 使用例

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/WaveletMatrix.hpp"
using namespace std;

int main() {
    vector<long long> a = {5, -2, 5, 1, 8, 3};
    WaveletMatrix<long long> wm(a);

    cout << wm.kth_smallest(1, 5, 1) << '\n'; // 1: {-2,1,5,8}
    cout << wm.range_freq(0, 6, 1, 6) << '\n'; // 4

    auto x = wm.prev_value(0, 6, 5);
    if (x) cout << *x << '\n';                 // 3
}
```

## 注意

- すべての区間は0-indexed半開区間です。
- `kth_*` に空区間を渡すことはできません。頻度クエリは空区間に対して `0` を返します。
- 座標圧縮後の値から元の `T` を復元して返します。
