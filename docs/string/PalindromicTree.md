# PalindromicTree

`libraries/string/PalindromicTree.hpp` は文字列に現れる異なる回文をノードとして管理するPalindromic Tree、別名Eertreeです。文字を末尾へ追加しながら構築できます。

## Include

```cpp
#include "libraries/string/PalindromicTree.hpp"
```

## Constructor

```cpp
PalindromicTree tree;
PalindromicTree tree(text);
```

空文字列から始めるか、`std::string` 全体から構築します。遷移を `std::map<char,int>` で保持するため、構築は `O(N log sigma)`、メモリは `O(N sigma)` ではなく実際に存在する遷移数に対して `O(N)` です。

## Node Layout

`nodes[0]` と `nodes[1]` は内部処理用の根です。非空回文に対応するノードは `[2,nodes.size())` にあります。

| member | description |
| --- | --- |
| `nodes[v].length` | 回文の長さ |
| `nodes[v].suffix_link` | 最長の真の回文接尾辞ノード |
| `nodes[v].first_end` | その回文が初めて現れた末尾位置 |
| `nodes[v].occurrences` | 伝播後の全文中の出現回数 |
| `nodes[v].next[c]` | 両端へ `c` を足した回文ノード |
| `suffix` | 現在の文字列全体の最長回文接尾辞ノード |
| `text` | 現在までに追加した文字列 |

## add

```cpp
int node = tree.add(char c);
```

文字列末尾へ `c` を追加し、新しい文字列の最長回文接尾辞ノードを返します。高々1つの新規ノードが作られます。

計算量: `O(log sigma)` 償却

## Queries

```cpp
int distinct_count() const;
string palindrome(int node) const;
```

`distinct_count()` は異なる非空回文数です。`palindrome(node)` はノードに対応する文字列を返し、文字列長に比例した時間がかかります。

## Occurrence Counts

```cpp
void propagate_occurrences();
```

長い回文からsuffix linkへ出現数を伝播し、各ノードの全文中での出現回数を確定します。計算量はノード数を `P` として `O(P log P)` です。

伝播は複数回呼んでも結果を重複加算しません。ただし、伝播後に `add` を呼ぶと `logic_error` になります。

## Example

```cpp
#include <bits/stdc++.h>
#include "libraries/string/PalindromicTree.hpp"
using namespace std;

int main() {
    PalindromicTree tree("ababa");
    cout << tree.distinct_count() << '\n'; // 5: a,b,aba,bab,ababa

    tree.propagate_occurrences();
    for (int node = 2; node < (int)tree.nodes.size(); ++node) {
        cout << tree.palindrome(node) << ' '
             << tree.nodes[node].occurrences << '\n';
    }
}
```

## Notes

- 同じ文字列を一括構築した場合と、1文字ずつ `add` した場合の結果は同じです。
- `clear()` で空文字列の状態へ戻せます。
- 回文の位置ごとの列挙や単純な回文判定だけなら、より軽量な `Manacher.hpp` が適しています。
