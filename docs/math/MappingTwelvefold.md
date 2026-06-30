# MappingTwelvefold

`libraries/math/MappingTwelvefold.hpp` は、写像12相の基本的な個数をまとめて計算します。

「玉」と「箱」が区別できるか、空箱を許すか、1箱に高々1個か、などの典型式をAPI名で呼べるようにしたものです。

## Include

```cpp
#include "libraries/math/MappingTwelvefold.hpp"
```

## Constructor

```cpp
MappingTwelvefold<T> mt(max_n);
```

`max_n` までの二項係数、第二種Stirling数、整数分割数を前計算します。

`T` は `+`、`*`、`+=`、`0`、`1` を扱える型です。modint にも使えます。

## Methods

名前は `balls_to_boxes_condition` です。

| method | balls | boxes | condition |
| --- | --- | --- | --- |
| `distinct_to_distinct_any(n,k)` | 区別あり | 区別あり | 制約なし |
| `distinct_to_distinct_injective(n,k)` | 区別あり | 区別あり | 高々1個 |
| `distinct_to_distinct_surjective(n,k)` | 区別あり | 区別あり | 空箱なし |
| `distinct_to_identical_any(n,k)` | 区別あり | 区別なし | 制約なし |
| `distinct_to_identical_injective(n,k)` | 区別あり | 区別なし | 高々1個 |
| `distinct_to_identical_surjective(n,k)` | 区別あり | 区別なし | 空箱なし |
| `identical_to_distinct_any(n,k)` | 区別なし | 区別あり | 制約なし |
| `identical_to_distinct_injective(n,k)` | 区別なし | 区別あり | 高々1個 |
| `identical_to_distinct_surjective(n,k)` | 区別なし | 区別あり | 空箱なし |
| `identical_to_identical_any(n,k)` | 区別なし | 区別なし | 制約なし |
| `identical_to_identical_injective(n,k)` | 区別なし | 区別なし | 高々1個 |
| `identical_to_identical_surjective(n,k)` | 区別なし | 区別なし | 空箱なし |

## Complexity

- Construction: `O(max_n^3)` for partition table
- Query: `O(1)` except small loops in `any` cases over identical boxes
- Memory: `O(max_n^2)`

## Example

```cpp
MappingTwelvefold<long long> mt(20);

cout << mt.distinct_to_distinct_any(3, 2) << '\n';        // 2^3 = 8
cout << mt.distinct_to_distinct_surjective(3, 2) << '\n'; // 6
cout << mt.identical_to_distinct_any(4, 3) << '\n';       // C(6,2) = 15
cout << mt.identical_to_identical_surjective(5, 2) << '\n'; // 2: 1+4, 2+3
```

## Notes

- `n` は玉の数、`k` は箱の数です。
- `max_n` より大きい入力は一部メソッドで `0` になります。必要な上限で構築してください。
- 「区別なし箱」の値は、箱の並べ替えを同一視します。
