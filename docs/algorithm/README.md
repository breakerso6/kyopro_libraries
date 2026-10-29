# Algorithm

[全ライブラリ一覧](../README.md) · [共通事項](../README.md#共通事項)

## [Mo](Mo.md)

```cpp
#include "libraries/algorithm/Mo.hpp"
```

半開区間クエリ `[l, r)` を並べ替え、現在区間との差分だけを適用します。

- `Mo mo(n)`: 配列長を指定
- `add_query(l, r)`: クエリを追加しIDを返す
- `run(add_left, add_right, erase_left, erase_right, answer)`: 全クエリを処理

要素1個の追加・削除を $O(F)$ とすると、典型的な計算量は $O((N+Q)\sqrt{N} F)$、メモリは $O(Q)$ です。

詳細は [Mo.md](Mo.md) を参照してください。

## [SMAWK](SMAWK.md)

```cpp
#include "libraries/algorithm/SMAWK.hpp"

auto columns = smawk(rows, cols, [&](int row, int col) { return cost(row, col); });
```

Totally Monotoneな行列について、各行の最小値を取る最左列を返します。行列を保持せず、値を返す関数を渡します。
比較可能な値を返す必要があり、行数を `N`、列数を `M` とすると評価回数・計算量は $O(N+M)$、返り値の長さは `N` です。列が空の場合はすべて `-1` になります。

詳細は [SMAWK.md](SMAWK.md) を参照してください。

## [Kitamasa](Kitamasa.md)

```cpp
#include "libraries/algorithm/Kitamasa.hpp"
```

線形漸化式 `a_n = c[0]a_{n-k} + ... + c[k-1]a_{n-1}` の第 `n` 項を `kitamasa(n, initial, c)` で求めます。係数ベクトルだけ欲しい場合は `kitamasa_coefficients(n, c)` を使います。計算量は $O(K^2 \log N)$ です。

詳細は [Kitamasa.md](Kitamasa.md) を参照してください。

## [Z-algorithm](ZAlgorithm.md)

```cpp
#include "libraries/algorithm/ZAlgorithm.hpp"
```

`z_algorithm(s)` は各位置からのLCP長を返します。`string` 以外にも `size()` と添字アクセスを持つコンテナに使えます。計算量は $O(N)$ です。

詳細は [ZAlgorithm.md](ZAlgorithm.md) を参照してください。

## [BeamSearch](BeamSearch.md)

```cpp
#include "libraries/algorithm/BeamSearch.hpp"
```

`CopySearch` / `TreeSearch` で状態のコピー保持・木上差分更新を選び、固定幅ビーム・chokudaiを実行します。通常の全体上位選抜、keyごとの上位選抜、全体上位へのkey別追加に対応します。keyを使わない通常版はkeyの取得・分類を行いません。

APIは [BeamSearch.md](BeamSearch.md)、問題側コード全体は [グリッド得点回収の6つの利用例とTODOひな型](../../examples/beam_search_draft/README.md) を参照してください。

## 設計案（未実装）

以下はAPI・動作を検討するためのドキュメントです。ヘッダや実装はまだなく、記載した使用例は現在のライブラリでは実行できません。

| ライブラリ | 内容 |
| --- | --- |
| [SimulatedAnnealing](SimulatedAnnealing.md) | 時間・反復数・温度・採否判定、差分評価・取り消し・評価打ち切りの利用例 |
