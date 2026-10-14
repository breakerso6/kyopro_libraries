# Random

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [Random.hpp](../../libraries/utility/Random.hpp)

`libraries/utility/Random.hpp` は競技プログラミング用の軽量な疑似乱数生成器です。

## include

```cpp
#include "libraries/utility/Random.hpp"
```

## コンストラクタ

```cpp
Random rng(uint64_t seed = 0);
```

seed `0` は固定の既定状態です。

## setSeed

```cpp
void rng.setSeed(uint64_t seed = 0);
```

seedを設定し直します。

## next / next64

```cpp
uint32_t rng.next();
uint64_t rng.next64();
```

32bit/64bit整数を返します。

### 計算量

- $O(1)$

## get / range / rangei

```cpp
size_t rng.get(size_t n);
size_t rng.range(size_t a, size_t b);
int64_t rng.rangei(int64_t a, int64_t b);
```

- `get(n)`: `[0,n)`
- `range(a,b)`: `[a,b)`
- `rangei(a,b)`: `[a,b)`

### 制約

- `get`: $0 < n \leq 2^{32}-1$
- `range`, `rangei`: $1 \leq b-a \leq 2^{32}-1$
- `rangei`: `b-a` が `int64_t` に収まる

### 計算量

- $O(1)$

## nextf / nextDouble / nextLog

```cpp
double rng.nextf();
double rng.nextDouble();
double rng.nextLog();
```

- `nextf()`, `nextDouble()`: `[0,1)`
- `nextLog()`: `log(nextDouble())`。生成値が `0` なら負の無限大を返します。

### 計算量

- $O(1)$

## shuffle

```cpp
template<class T>
void rng.shuffle(vector<T>& a);
```

Fisher-Yates shuffleを行います。

### 制約

- $N \leq 2^{32}-1$
- 要素を交換できる

### 計算量

- $O(N)$

## 注意

- 暗号用途には使用しないでください。

## nextInt / nextUInt32 / range_skip

```cpp
int rng.nextInt(int m);
uint32_t rng.nextUInt32();
size_t rng.range_skip(size_t a, size_t b, size_t skip);
```

`nextInt(m)` は `[0,m)`、`nextUInt32()` は `next()` と同じ32bit整数を返します。`range_skip` は `[a,b)` から `skip` を除いた整数を返します。

### 制約

- `nextInt`: $1 \leq m$
- `range_skip`: `a <= skip < b`、$2 \leq b-a \leq 2^{32}$
- `range_skip` の幅は `size_t` に収まる

### 計算量

- $O(1)$。コンストラクタと `setSeed` も $O(1)$

## 使用例

```cpp
#include "libraries/utility/Random.hpp"
using namespace std;

int main() {
    Random first(42), second(42);
    assert(first.next() == second.next());
    int value = first.nextInt(10);
    assert(0 <= value && value < 10);
    vector<int> values = {1, 2, 3};
    first.shuffle(values);
}
```
