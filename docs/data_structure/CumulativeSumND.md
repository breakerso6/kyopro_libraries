# CumulativeSumND

`libraries/data_structure/CumulativeSumND.hpp` は静的な n 次元配列に対する直方体和を扱います。

## Include

```cpp
#include "libraries/data_structure/CumulativeSumND.hpp"
```

## コンストラクタ

```cpp
CumulativeSumND<T> cs(shape);
CumulativeSumND<T> cs(shape, values);
```

`shape` は各次元の長さです。`values` は最後の次元が最も速く動く順の1次元配列です。

**制約**

- `D = shape.size()` は小さいこと
- `values.size()` は `shape` の積
- `T` は `T()`, `+=`, `-=`, `+`, `-` が使える

**計算量**

- 構築: $O(D \prod_i (shape_i+1))$
- メモリ: $O(\prod_i (shape_i+1))$

## add

```cpp
void add(vector<int> index, T value);
```

`build()` 前に、1点 `index` へ `value` を加算します。

**計算量**

- $O(D)$

## build

```cpp
void build();
```

累積和を構築します。`shape` のみで作った場合は、必要な点を `add` してから呼びます。`values` 付きコンストラクタでは自動で呼ばれます。

**計算量**

- $O(D \prod_i (shape_i+1))$

## sum

```cpp
T sum(vector<int> lower, vector<int> upper) const;
```

n 次元直方体 `prod_i [lower_i, upper_i)` の和を返します。

**計算量**

- $O(D 2^D)$

## 使用例

```cpp
vector<int> shape{3, 4, 5};
vector<long long> a(3 * 4 * 5);

auto id = [&](int x, int y, int z) {
    return (x * shape[1] + y) * shape[2] + z;
};
a[id(1, 2, 3)] = 10;

CumulativeSumND<long long> cs(shape, a);
cout << cs.sum({1, 2, 3}, {2, 3, 4}) << '\n'; // 10
cout << cs.sum({0, 0, 0}, {3, 4, 5}) << '\n'; // 全体の和
```

## 注意

- すべての区間は半開区間です。
- 静的構造です。`build()` 後の更新はできません。
- 次元数が大きいと `sum` の包除原理が重くなります。
