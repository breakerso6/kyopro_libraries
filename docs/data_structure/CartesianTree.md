# CartesianTree

`libraries/data_structure/CartesianTree.hpp` は配列の添字順を中間順巡回として保つヒープ順序付き二分木を `O(N)` で構築します。既定では最小Cartesian Treeです。

## Include

```cpp
#include "libraries/data_structure/CartesianTree.hpp"
```

## Template Parameters

```cpp
template<class T, class Compare = std::less<T>>
struct CartesianTree;
```

- `T`: 配列要素型
- `Compare`: 親に置く値を判定する比較。`less<T>` なら最小、`greater<T>` なら最大Cartesian Tree

## Constructor

```cpp
CartesianTree<T, Compare> tree(values, compare);
```

`compare` は省略できます。構築時間・メモリは `O(N)` です。

## Public Members

| member | description |
| --- | --- |
| `root` | 根の添字。空配列なら `-1` |
| `parent[v]` | 親。根は `-1` |
| `left[v]` | 左の子。存在しなければ `-1` |
| `right[v]` | 右の子。存在しなければ `-1` |

## build

```cpp
void build(const vector<T>& values, Compare compare = Compare());
```

既存インスタンスを新しい配列で再構築します。

## Example

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/CartesianTree.hpp"
using namespace std;

int main() {
    vector<int> a = {3, 1, 4, 0, 2};
    CartesianTree<int> tree(a);

    cout << tree.root << '\n';      // 3
    cout << tree.parent[1] << '\n'; // 3
    cout << tree.right[3] << '\n';  // 4
}
```

## Properties and Notes

- 各辺 `parent -> child` で、既定では `a[parent] <= a[child]` です。
- 中間順巡回すると添字 `0,1,...,N-1` が得られます。
- 同値要素では先に現れた要素を上側に保ちます。
- RMQとCartesian TreeのLCAを対応させる用途や、各要素が区間最小値になる最大範囲の解析に利用できます。
