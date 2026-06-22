# Math

## XorBasis

```cpp
#include "libraries/math/XorBasis.hpp"
```

整数列のXOR線形基底を管理します。

- `insert(x)`: rankが増えた場合に `true`
- `contains(x)`: 挿入値の部分集合XORで表現可能か
- `max_xor(seed)`: `seed` から得られる最大XOR
- `rank`: 基底のrank

各操作は `O(BITS)`、メモリは `O(BITS)` です。
