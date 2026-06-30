# IndexSet

`libraries/data_structure/Indexset.hpp` は `0, 1, ..., n - 1` の部分集合を管理するライブラリです。

要素の追加、削除、所属判定、ランダム取得を `O(1)` で行えます。

## Include

```cpp
#include "libraries/data_structure/Indexset.hpp"
```

`libraries/utility/Random.hpp` を内部で include し、グローバル変数 `rnd` を使ってランダム取得します。

## コンストラクタ

```cpp
IndexSet s(n);
```

- `n`: 管理する添字の数
- 初期状態では `0, 1, ..., n - 1` をすべて含みます。

**制約**

- $0 \leq n$

**計算量**

- 構築: `O(N)`
- メモリ: `O(N)`

## 関数

### fill

```cpp
void fill();
```

集合を `{0, 1, ..., n - 1}` に戻します。

**計算量**

- `O(1)`

### clear

```cpp
void clear();
```

集合を空にします。

**計算量**

- `O(1)`

### size

```cpp
int size();
```

現在含まれている要素数を返します。

**計算量**

- `O(1)`

### contain

```cpp
bool contain(int i);
```

`i` が集合に含まれているかを返します。

**制約**

- $0 \leq i < n$

**計算量**

- `O(1)`

### push

```cpp
void push(int i);
```

`i` を集合に追加します。すでに含まれている場合は何もしません。

**制約**

- $0 \leq i < n$

**計算量**

- `O(1)`

### erase

```cpp
void erase(int i);
```

`i` を集合から削除します。含まれていない場合は何もしません。

**制約**

- $0 \leq i < n$

**計算量**

- `O(1)`

### random

```cpp
int random();
```

集合に含まれる要素をランダムに 1 つ返します。

集合が空の場合は `-1` を返します。

**計算量**

- `O(1)`

## 使用例

```cpp
#include "libraries/data_structure/Indexset.hpp"

int main() {
    IndexSet s(5);  // {0, 1, 2, 3, 4}

    s.erase(2);     // {0, 1, 3, 4}
    cout << s.contain(2) << '\n';  // 0
    cout << s.size() << '\n';      // 4

    s.push(2);      // {0, 1, 2, 3, 4}

    int x = s.random();
    cout << (0 <= x && x < 5) << '\n';  // 1

    s.clear();
    cout << s.random() << '\n';  // -1
}
```
