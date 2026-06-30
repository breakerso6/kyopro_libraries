# ZAlgorithm

`libraries/algorithm/ZAlgorithm.hpp` は列 `s` の各位置について、`s[0...]` と `s[i...]` の最長共通接頭辞長を求めます。

## Include

```cpp
#include "libraries/algorithm/ZAlgorithm.hpp"
```

## 関数

```cpp
template<class Container>
vector<int> z_algorithm(const Container& s);
```

`s.size()` と `s[i]` が使えるコンテナに対応します。`string`、`vector<int>` などに使えます。

返り値 `z` は次の意味です。

- `z[0] = s.size()`
- `z[i] = lcp(s, s.substr(i))`

**制約**

- `s.size()` と `s[i]` が定義されている

**計算量**

- $O(N)$

## 使用例

```cpp
#include <bits/stdc++.h>
#include "libraries/algorithm/ZAlgorithm.hpp"
using namespace std;

int main() {
    string s = "ababa";
    auto z = z_algorithm(s);
    // {5, 0, 3, 0, 1}
}
```

## パターン検索

文字列 `text` に `pattern` が出る位置を探す典型例です。

```cpp
string pattern = "aba";
string text = "abacababa";
string combined = pattern + "$" + text;
auto z = z_algorithm(combined);

for (int i = 0; i < (int)text.size(); ++i) {
    if (z[pattern.size() + 1 + i] >= (int)pattern.size()) {
        cout << i << '\n';
    }
}
```

区切り文字 `$` は `pattern` と `text` に出ない文字を使います。
