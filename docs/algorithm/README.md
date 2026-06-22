# Algorithm

## Mo

```cpp
#include "libraries/algorithm/Mo.hpp"
```

半開区間クエリ `[l, r)` を並べ替え、現在区間との差分だけを適用します。

- `Mo mo(n)`: 配列長を指定
- `add_query(l, r)`: クエリを追加しIDを返す
- `run(add_left, add_right, erase_left, erase_right, answer)`: 全クエリを処理

要素1個の追加・削除を `O(F)` とすると、典型的な計算量は `O((N+Q)sqrt(N) F)`、メモリは `O(Q)` です。
