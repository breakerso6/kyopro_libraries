# AhoCorasick

`libraries/string/AhoCorasick.hpp` は複数パターンをTrieに登録し、テキスト中の各パターン出現回数をまとめて数えます。

## Include

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

**制約**

- 各文字 `c` について `0 <= c - FIRST < SIGMA`
- `build()` の前に呼ぶ

**計算量**

- $O(|pattern| \log 1)$ 程度。実装上は各文字 $O(1)$

## build

```cpp
ac.build();
```

failure linkを構築します。

**計算量**

- $O(ノード数 \cdot SIGMA + 出力伝播量)$

## count_matches

```cpp
vector<int> count = ac.count_matches(string_view text) const;
```

`count[id]` にパターン `id` の出現回数を返します。

**制約**

- `build()` を呼び出し済み
- 各文字 `c` について `0 <= c - FIRST < SIGMA`

**計算量**

- $O(|text| + 出力数)$

## 使用例

```cpp
AhoCorasick<> ac;
vector<string> patterns = {"he", "she", "hers"};
for (auto& p : patterns) ac.add(p);
ac.build();

auto count = ac.count_matches("ahishers");
```
