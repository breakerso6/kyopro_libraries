# RollbackDSU

`libraries/data_structure/RollbackDSU.hpp` は直前のmergeを取り消せるUnion-Findです。オフライン動的連結性や分割統治上の辺追加に使います。

## Include

```cpp
#include "libraries/data_structure/RollbackDSU.hpp"
```

## コンストラクタ

```cpp
RollbackDSU dsu(int n);
```

$n$ 頂点 $0$ 辺の無向グラフを作ります。

**制約**

- $0 \leq n$

**計算量**

- $O(n)$

## merge

```cpp
bool dsu.merge(int a, int b);
```

辺 `(a,b)` を追加します。新たに連結したなら `true`、すでに連結なら `false` です。

**制約**

- $0 \leq a,b < n$

**計算量**

- $O(\log n)$

## undo / snapshot / rollback

```cpp
bool dsu.undo();
int state = dsu.snapshot() const;
void dsu.rollback(int state);
```

- `undo`: 直前に成功した `merge` を1回取り消す
- `snapshot`: 現在の履歴位置を返す
- `rollback`: 指定した履歴位置まで戻す

**制約**

- `rollback` に渡す `state` は過去に `snapshot()` が返した値

**計算量**

- `undo`: $O(1)$
- `rollback`: 戻すmerge数に比例

## leader / same / size

```cpp
int dsu.leader(int v) const;
bool dsu.same(int a, int b) const;
int dsu.size(int v) const;
```

代表元、連結判定、連結成分サイズを返します。

**計算量**

- $O(\log n)$

## 注意

- 経路圧縮はしません。
- すでに連結な頂点への `merge` は履歴を積まないため、`undo` の対象になりません。
