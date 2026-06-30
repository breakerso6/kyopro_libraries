# ConvexHullTrick

`libraries/data_structure/ConvexHullTrick.hpp` は直線 `y = ax + b` の集合に対して、指定した `x` での最小値または最大値を求めます。

直線の傾きが単調に追加される場合に使います。任意順に直線を追加したい場合は `LiChaoTree.hpp` を使います。

## Include

```cpp
#include "libraries/data_structure/ConvexHullTrick.hpp"
```

## コンストラクタ

```cpp
ConvexHullTrick<T> cht(true);   // minimum
ConvexHullTrick<T> cht(false);  // maximum
```

省略時は最小値クエリです。

**計算量**

- $O(1)$

## add_line

```cpp
void add_line(T a, T b);
```

直線 `ax+b` を追加します。

**制約**

- 最小値クエリ: 傾き `a` を降順に追加
- 最大値クエリ: 傾き `a` を昇順に追加

同じ傾きの直線は、より良い切片だけが残ります。

**計算量**

- ならし $O(1)$

## query

```cpp
T query(T x) const;
```

現在の直線集合での最小値または最大値を返します。

**制約**

- 1本以上の直線が追加されている
- `a*x+b` が `T` の範囲に収まる

**計算量**

- $O(\log N)$

## 使用例

```cpp
ConvexHullTrick<long long> cht;
cht.add_line(5, -20);
cht.add_line(2, -5);
cht.add_line(-1, 2);

cout << cht.query(3) << '\n'; // min(15-20, 6-5, -3+2) = -5
```

## 注意

- `query` の `x` は任意順で呼べます。
- 直線追加順の前提を破ると答えは壊れます。
- 積 `a*x+b` が `T` の範囲に収まる必要があります。
