# SWAG

`libraries/data_structure/SWAG.hpp` はキューに対する総積を管理するSliding Window Aggregationです。

## Include

```cpp
#include "libraries/data_structure/SWAG.hpp"
```

## コンストラクタ

```cpp
SWAG<S, Op> swag(identity, op);
```

**制約**

- `Op` は結合的
- `identity` は `Op` の単位元

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
SWAG<long long, Sum> swag(0);
swag.push(1);
swag.push(2);
cout << swag.prod() << '\n'; // 3
swag.pop();
```
