# SplayTreeSequence

`libraries/data_structure/SplayTreeSequence.hpp` は列をSplay Treeとして管理し、位置指定の挿入・削除・区間反転・区間積を行うデータ構造です。

## Include

```cpp
#include "libraries/data_structure/SplayTreeSequence.hpp"
```

## テンプレート引数

```cpp
template<class S, S (*op)(S, S), S (*e)()>
struct SplayTreeSequence;
```

| parameter | meaning |
| --- | --- |
| `S` | 要素・区間積の型 |
| `op` | `S op(S, S)` 形式の結合的な演算 |
| `e` | 単位元を返す `S e()` |

**制約**

- `op` は結合的
- `e()` は `op` の単位元

## コンストラクタ

```cpp
SplayTreeSequence<S, op, e> splay;
SplayTreeSequence<S, op, e> splay(values);
```

`values` を渡した場合は、先頭から順に列を構築します。

**計算量**

- 空構築: `O(1)`
- `values` から構築: ならし `O(N log N)`

## 関数

### size / empty

```cpp
int size() const;
bool empty() const;
```

列の長さ、空判定を返します。

**計算量**

- `O(1)`

### insert / erase

```cpp
void insert(int position, const S& value);
void erase(int position);
```

`insert` は `position` 番目の直前へ `value` を挿入します。`erase` は `position` 番目の要素を削除します。

**制約**

- `insert`: $0 \leq position \leq N$
- `erase`: $0 \leq position < N$

**計算量**

- ならし `O(log N)`

### set / get

```cpp
void set(int position, const S& value);
S get(int position);
```

`set` は `position` 番目の要素を `value` に置き換えます。`get` は `position` 番目の要素を返します。

**制約**

- $0 \leq position < N$

**計算量**

- ならし `O(log N)`

### prod

```cpp
S prod(int l, int r);
```

半開区間 `[l,r)` の `op` による積を返します。`l = r` のときは `e()` を返します。

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- ならし `O(log N)`

### reverse

```cpp
void reverse(int l, int r);
```

半開区間 `[l,r)` の並びを反転します。

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- ならし `O(log N)`

### to_vector

```cpp
vector<S> to_vector();
```

現在の列を `vector` として返します。

**計算量**

- `O(N)`

## 使用例

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/SplayTreeSequence.hpp"
using namespace std;

long long op(long long a, long long b) {
    return a + b;
}

long long e() {
    return 0;
}

int main() {
    SplayTreeSequence<long long, op, e> splay({1, 2, 3, 4, 5});

    splay.reverse(1, 4);              // {1,4,3,2,5}
    cout << splay.prod(0, 3) << '\n'; // 8

    splay.insert(2, 10);              // {1,4,10,3,2,5}
    splay.erase(3);                   // {1,4,10,2,5}
    splay.set(0, 7);                  // {7,4,10,2,5}
}
```

## 注意

- すべての区間は0-indexed半開区間です。
- Splay Treeなので、各操作の計算量はならしです。
- 非可換な `op` にも対応するため、反転後の `prod` は反転された順序で計算されます。
