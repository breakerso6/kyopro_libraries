# WeightedDSU

`libraries/data_structure/WeightedDSU.hpp` は各頂点にポテンシャルを持つUnion-Findです。制約 `potential(b) - potential(a) = w` を追加し、差分を問い合わせます。

## Include

```cpp
#include "libraries/data_structure/WeightedDSU.hpp"
```

## コンストラクタ

```cpp
WeightedDSU<T> dsu(int n);
```

**制約**

- $0 \leq n$
- `T` は加減算と単位元 `T{}` を扱える

## merge

```cpp
bool dsu.merge(int a, int b, T w);
```

制約 `potential(b) - potential(a) = w` を追加します。既存制約と矛盾する場合は `false` を返します。

**制約**

- $0 \leq a,b < n$

**計算量**

- ならし $O(\alpha(n))$

## potential / diff / same / size

```cpp
T dsu.potential(int v);
T dsu.diff(int a, int b);
bool dsu.same(int a, int b);
int dsu.size(int v);
```

- `potential(v)`: 代表元から見た `v` のポテンシャル
- `diff(a,b)`: `potential(b) - potential(a)`
- `same(a,b)`: 同じ連結成分か
- `size(v)`: 連結成分サイズ

**制約**

- `diff(a,b)` は `same(a,b)` が `true` のときに使う

**計算量**

- ならし $O(\alpha(n))$

## 使用例

```cpp
WeightedDSU<long long> dsu(3);
dsu.merge(0, 1, 5);
dsu.merge(1, 2, -2);

cout << dsu.diff(0, 2) << '\n'; // 3
```
