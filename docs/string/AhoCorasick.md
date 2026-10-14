# AhoCorasick

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [AhoCorasick.hpp](../../libraries/string/AhoCorasick.hpp)

`libraries/string/AhoCorasick.hpp` は複数パターンをTrieに登録し、テキスト中の各パターン出現回数をまとめて数えます。

## include

```cpp
#include "libraries/string/AhoCorasick.hpp"
```

## コンストラクタ

```cpp
AhoCorasick<SIGMA, FIRST> ac;
```

既定では `SIGMA=26`, `FIRST='a'` です。

## add

```cpp
int id = ac.add(string_view pattern);
```

パターンを追加し、パターンIDを返します。

### 制約

- 各文字 `c` について `0 <= c - FIRST < SIGMA`
- `build()` の前に呼ぶ

### 計算量

- $O(|pattern| \log 1)$ 程度。実装上は各文字 $O(1)$

## build

```cpp
ac.build();
```

failure linkを構築します。

### 計算量

- $O(ノード数 \cdot SIGMA + 出力伝播量)$

## count_matches

```cpp
vector<int> count = ac.count_matches(string_view text) const;
```

`count[id]` にパターン `id` の出現回数を返します。

### 制約

- `build()` を呼び出し済み
- 各文字 `c` について `0 <= c - FIRST < SIGMA`

### 計算量

- $O(|text| + ノード数 + パターン数)$

一致が多い場合は、各状態への訪問回数を failure link に沿って逆順に集約します。すべての状態で出力が4個以下の場合は直接集計し、$O(|text| + パターン数)$ で処理します。短い入力で出力数の上限がノード数以下の場合も、直接集計してTrie全体の走査を省きます。

## 使用例

```cpp
AhoCorasick<> ac;
vector<string> patterns = {"he", "she", "hers"};
for (auto& p : patterns) ac.add(p);
ac.build();

auto count = ac.count_matches("ahishers");
```
