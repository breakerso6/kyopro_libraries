# Manacher

`libraries/string/Manacher.hpp` は文字列中の各位置を中心とする最長回文半径を線形時間で計算します。前処理後は任意の部分文字列が回文かを `O(1)` で判定できます。

## Include

```cpp
#include "libraries/string/Manacher.hpp"
```

## コンストラクタ

```cpp
Manacher manacher(text);
```

`text` は `std::string` です。

**制約**

- $0 \leq N$

- 構築: `O(N)`
- メモリ: `O(N)`

既存インスタンスは `build(text)` で再構築できます。

## メンバ変数

| member | description |
| --- | --- |
| `odd[i]` | `i` を中心とする奇数長回文の半径 |
| `even[i]` | `i-1` と `i` の境界を中心とする偶数長回文の半径 |

奇数半径 `k` は区間 `[i-k+1,i+k)` が回文であることを表します。偶数半径 `k` は `[i-k,i+k)` が回文です。

例えば `text="abac"` では、`odd[1]=2` が部分文字列 `"aba"` を表します。

## is_palindrome

```cpp
bool is_palindrome(int l, int r) const;
```

半開部分文字列 `text[l,r)` が回文かを返します。空文字列は回文として `true` です。

**制約**

- $0 \leq l \leq r \leq N$

**計算量**

- `O(1)`

## 使用例

```cpp
#include <bits/stdc++.h>
#include "libraries/string/Manacher.hpp"
using namespace std;

int main() {
    string s = "abacaba";
    Manacher manacher(s);

    cout << manacher.odd[3] << '\n';          // 4
    cout << manacher.is_palindrome(0, 7) << '\n'; // 1
    cout << manacher.is_palindrome(1, 5) << '\n'; // 0

    long long palindrome_count = 0;
    for (int radius : manacher.odd) palindrome_count += radius;
    for (int radius : manacher.even) palindrome_count += radius;
    cout << palindrome_count << '\n';
}
```

## 注意

- 添字は0-indexed、判定区間は半開区間です。
- `odd` と `even` の半径の総和は、位置を区別した非空回文部分文字列の総数です。
- 異なる回文文字列の種類数や出現回数を管理したい場合は `PalindromicTree.hpp` を使います。
