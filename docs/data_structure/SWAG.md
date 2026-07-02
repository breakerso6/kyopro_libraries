# SWAG

`libraries/data_structure/SWAG.hpp` はキューに対する総積を管理するSliding Window Aggregationです。

## Include

```cpp
#include "libraries/data_structure/SWAG.hpp"
```

## コンストラクタ

```cpp
SWAG<S, op, e> swag;
```

**制約**

- `S op(S a, S b)` は結合的
- `S e()` は `op` の単位元

## push / pop

```cpp
void swag.push(const S& x);
void swag.pop();
```

末尾へ追加、先頭から削除します。

**制約**

- `pop`: `!swag.empty()`

**計算量**

- ならし $O(1)$

## prod / size / empty

```cpp
S swag.prod() const;
int swag.size() const;
bool swag.empty() const;
```

現在のキュー全体の積、要素数、空判定を返します。

**計算量**

- $O(1)$

## 使用例

```cpp
long long op(long long a, long long b) {
    return a + b;
}

long long e() {
    return 0;
}

SWAG<long long, op, e> swag;
swag.push(1);
swag.push(2);
cout << swag.prod() << '\n'; // 3
swag.pop();
```
