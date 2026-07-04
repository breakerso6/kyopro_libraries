# Compress

`libraries/data_structure/Compress.hpp` は座標圧縮を行うライブラリです。

値をソートして重複を除いた列を作り、各値を `0, 1, ..., size() - 1` の添字へ変換します。

## Include

```cpp
#include "libraries/data_structure/Compress.hpp"
```

## コンストラクタ

```cpp
Compress<T> comp;
Compress<T> comp(xs);
```

- `xs`: 圧縮したい値の列
- `values`: ソート済み・重複除去済みの値列

**計算量**

- 構築: `O(N log N)`
- メモリ: `O(N)`

## 関数

### add

```cpp
void add(const T& x);
void add(const vector<T>& xs);
```

圧縮候補の値を追加します。追加後は `build()` を呼んでください。

**計算量**

- `add(x)`: `O(1)` 償却
- `add(xs)`: `O(xs.size())`

### build

```cpp
void build();
void build(const vector<T>& xs);
```

追加済みの値、または `xs` から圧縮テーブルを作ります。

**計算量**

- `O(N log N)`

### size / empty

```cpp
int size() const;
bool empty() const;
```

圧縮後の値の個数と、空かどうかを返します。

**計算量**

- `O(1)`

### operator[]

```cpp
const T& operator[](int i) const;
```

圧縮後の添字 `i` に対応する元の値を返します。

**制約**

- `0 <= i < size()`

**計算量**

- `O(1)`

### lower_bound / upper_bound

```cpp
int lower_bound(const T& x) const;
int upper_bound(const T& x) const;
```

`values` 上での `lower_bound` / `upper_bound` の位置を返します。

**計算量**

- `O(log N)`

### contains

```cpp
bool contains(const T& x) const;
```

`x` が圧縮テーブルに含まれるかを返します。

**計算量**

- `O(log N)`

### index / operator()

```cpp
int index(const T& x) const;
int operator()(const T& x) const;
```

`x` の圧縮後の添字を返します。`x` は圧縮テーブルに含まれている必要があります。

**制約**

- `contains(x) == true`

**計算量**

- `O(log N)`

### compressed

```cpp
vector<int> compressed(const vector<T>& xs) const;
```

`xs` の各値を圧縮後の添字に変換した列を返します。各値は圧縮テーブルに含まれている必要があります。

**計算量**

- `O(M log N)`

## 使用例

```cpp
#include "libraries/data_structure/Compress.hpp"

int main() {
    vector<long long> x{100, -5, 100, 7};
    Compress<long long> comp(x);

    auto a = comp.compressed(x);  // {2, 0, 2, 1}
    cout << comp.size() << '\n';  // 3
    cout << comp(-5) << '\n';     // 0
    cout << comp[2] << '\n';      // 100
    cout << comp.lower_bound(8) << '\n';  // 2
}
```
