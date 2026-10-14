# Mo

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [Mo.hpp](../../libraries/algorithm/Mo.hpp)

`libraries/algorithm/Mo.hpp` は静的配列上の半開区間クエリ `[l,r)` を並べ替え、区間の左右端を少しずつ動かして処理します。

## include

```cpp
#include "libraries/algorithm/Mo.hpp"
```

## コンストラクタ

```cpp
Mo mo(int n);
```

配列長 `n` を指定します。

### 制約

- $0 \leq n$

## add_query

```cpp
int id = mo.add_query(int l, int r);
```

クエリ `[l,r)` を追加し、クエリIDを返します。

### 制約

- $0 \leq l \leq r \leq n$

### 計算量

- $O(1)$

## run

```cpp
mo.run(add_left, add_right, erase_left, erase_right, answer);
```

クエリをMo順に処理します。

左端のブロック番号は事前に計算し、ソートの比較関数内での整数除算を省きます。クエリ順序の規則は同じです。

- `add_left(i)`: 左端を左へ動かして `i` を追加
- `add_right(i)`: 右端を右へ動かして `i` を追加
- `erase_left(i)`: 左端を右へ動かして `i` を削除
- `erase_right(i)`: 右端を左へ動かして `i` を削除
- `answer(id)`: クエリ `id` の答えを保存

### 制約

- 追加・削除関数は現在区間の状態を正しく更新する

### 計算量

- 追加・削除1回の計算量を $F$ として $O((N+Q)\sqrt Q F)$ 程度

## 使用例

区間内の異なる値の個数を求めます。

```cpp
Mo mo(n);
for (auto [l, r] : queries) mo.add_query(l, r);

vector<int> ans(queries.size()), freq(max_value);
int distinct = 0;
auto add = [&](int i) { if (freq[a[i]]++ == 0) ++distinct; };
auto erase = [&](int i) { if (--freq[a[i]] == 0) --distinct; };

mo.run(add, add, erase, erase, [&](int id) {
    ans[id] = distinct;
});
```

## 注意

- 配列の値は途中で変わらない前提です。
- 更新つきMoには対応していません。
