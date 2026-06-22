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

## [SMAWK](SMAWK.md)

```cpp
#include "libraries/algorithm/SMAWK.hpp"

auto columns = smawk(rows, cols, [&](int row, int col) { return cost(row, col); });
```

Totally Monotoneな行列について、各行の最小値を取る最左列を返します。行列を保持せず、値を返す関数を渡します。
比較可能な値を返す必要があり、行数を `N`、列数を `M` とすると評価回数・計算量は `O(N+M)`、返り値の長さは `N` です。列が空の場合はすべて `-1` になります。

詳細は [SMAWK.md](SMAWK.md) を参照してください。
