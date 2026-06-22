# Utility

## Random

```cpp
#include "libraries/utility/Random.hpp"
```

軽量な疑似乱数生成器です。`Random rng(seed)` で構築します。seed `0` は固定の既定状態です。

- `next`, `next64`: 32/64 bit整数
- `get(n)`, `nextInt(n)`: `[0,n)`
- `range(a,b)`, `rangei(a,b)`: `[a,b)`
- `nextDouble`: `[0,1)`
- `shuffle(vector)`: Fisher-Yates shuffle

暗号用途には使用しないでください。
