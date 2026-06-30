# Random

`libraries/utility/Random.hpp` は競技プログラミング用の軽量な疑似乱数生成器です。

## Include

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

**計算量**

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

**制約**

- `get`: $0 < n \leq 2^{32}-1$
- `range`, `rangei`: `a < b`

**計算量**

- $O(1)$

## nextDouble / nextLog

```cpp
double rng.nextDouble();
double rng.nextLog();
```

- `nextDouble()`: `[0,1)`
- `nextLog()`: `log(nextDouble())`

## shuffle

```cpp
template<class T>
void rng.shuffle(vector<T>& a);
```

Fisher-Yates shuffleを行います。

**計算量**

- $O(N)$

## 注意

- 暗号用途には使用しないでください。
