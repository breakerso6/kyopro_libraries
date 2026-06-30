# Utility

## [Random](Random.md)

```cpp
#include "libraries/utility/Random.hpp"
```

軽量な疑似乱数生成器です。`Random rng(seed)` で構築します。seed `0` は固定の既定状態です。

- `next`, `next64`: 32/64 bit整数
- `get(n)`, `nextInt(n)`: `[0,n)`
- `range(a,b)`, `rangei(a,b)`: `[a,b)`
- `nextDouble`: `[0,1)`
- `shuffle(vector)`: Fisher-Yates shuffle

## 使用例

```cpp
#include "libraries/utility/Random.hpp"

Random rng(12345);
int x = rng.get(10);          // [0, 10)
long long y = rng.range(5, 9); // [5, 9)
rng.shuffle(values);
```

暗号用途には使用しないでください。

詳細は [Random.md](Random.md) を参照してください。
