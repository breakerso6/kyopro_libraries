# SMAWK

`libraries/algorithm/SMAWK.hpp` はTotally Monotoneな行列について、各行の最小要素がある最左列を求めます。行列を保存せず、要素を評価する関数を渡します。

## Include

```cpp
#include "libraries/algorithm/SMAWK.hpp"
```

## API

```cpp
template<class F>
vector<int> smawk(int rows, int columns, F value);
```

- `rows`: 行数
- `columns`: 列数
- `value(row,col)`: 行列要素を返す関数
- 返り値: `answer[row]` がその行の最小値を取る最左列

`value` の返り値には `<` が定義されている必要があります。同値な最小値が複数ある場合は小さい列番号を返します。

- 評価回数・計算量: `O(rows + columns)`
- メモリ: `O(rows + columns)`

列数が `0` の場合、返り値はすべて `-1` です。

## Required Property

行列 `A` がTotally Monotoneである必要があります。任意の `i1 < i2`, `j1 < j2` について、次が成り立つことが条件です。

```text
A[i1][j1] > A[i1][j2] ならば A[i2][j1] > A[i2][j2]
```

Monge行列はTotally Monotoneなので適用できます。条件を満たさない行列を渡した場合、返り値の正しさは保証されません。

## Example

各行 `i` で `(j-floor(3i/2))^2+i` を最小化します。

```cpp
#include <bits/stdc++.h>
#include "libraries/algorithm/SMAWK.hpp"
using namespace std;

int main() {
    int n = 5, m = 8;
    auto cost = [](int i, int j) {
        long long d = j - (3 * i / 2);
        return d * d + i;
    };

    vector<int> argmin = smawk(n, m, cost);
    for (int column : argmin) cout << column << ' ';
    cout << '\n'; // 0 1 3 4 6
}
```

## Typical DP Use

遷移コスト行列がMonge性を持つDPでは、行を状態、列を遷移元として `value` 内で遷移値を計算できます。評価関数は何度も呼ばれるため、1回の評価が `O(1)` になるよう前計算してください。

## Notes

- 最小化専用です。最大化したい場合は評価値の符号を反転するか、大小関係を反転するラッパー型を使います。
- `value` は同じ要素に複数回呼ばれる可能性があるため、副作用を持たせないでください。
- 行・列はともに0-indexedです。
