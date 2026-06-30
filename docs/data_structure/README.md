# Data structures

区間クエリ、集合管理、永続化、オフライン処理をまとめています。迷ったら次の基準で選びます。

- 静的RMQ: `SparseTable`
- 静的一般モノイド区間積: `DisjointSparseTable`
- 一点更新 + 区間積: 通常のセグ木が必要。広い疎座標なら `DynamicSegmentTree`
- 区間 `chmin/chmax/add` + min/max/sum: `SegmentTreeBeats`
- 過去versionを参照する: `Persistent*`
- 直線最小値: 傾き単調なら `ConvexHullTrick`、任意順なら `LiChaoTree`
- 値域頻度・k番目: `WaveletMatrix`

| library | 主な関数 | 計算量 |
| --- | --- | --- |
| [BinaryTrie.hpp](BinaryTrie.md) | `insert`, `erase`, `count`, `min_xor`, `max_xor` | 1操作 `O(BITS)` |
| [RollbackDSU.hpp](RollbackDSU.md) | `merge`, `undo`, `snapshot`, `rollback` | `merge`/`undo`: `O(log N)` |
| [WeightedDSU.hpp](WeightedDSU.md) | `merge(a,b,w)`, `diff`, `same` | 償却 `O(alpha(N))` |
| [SparseTable.hpp](SparseTable.md) | `prod(l,r)` | 構築 `O(N log N)`、取得 `O(1)` |
| [DisjointSparseTable.hpp](DisjointSparseTable.md) | `prod(l,r)` | 構築 `O(N log N)`、取得 `O(1)` |
| [SWAG.hpp](SWAG.md) | `push`, `pop`, `prod` | 償却 `O(1)` |
| [LiChaoTree.hpp](LiChaoTree.md) | `add_line(a,b)`, `min(x)` | `O(log X)` |
| [ConvexHullTrick.hpp](ConvexHullTrick.md) | `add_line(a,b)`, `query(x)` | 追加 `O(1)` 償却、取得 `O(log N)` |
| [IntervalSet.hpp](IntervalSet.md) | `insert`, `erase`, `contains`, `covers`, `mex` | 変更区間数に依存、検索 `O(log N)` |
| [Indexset.hpp](Indexset.md) | `push`, `erase`, `contain`, `random` | 1操作 `O(1)` |
| [PersistentSegmentTree.hpp](Persistence.md#persistentsegmenttree) | `build`, `set`, `prod`, `get` | 取得・更新 `O(log N)` |
| [DynamicSegmentTree.hpp](DynamicSegmentTree.md) | `set`, `get`, `prod` | 取得・更新 `O(log X)` |
| [PersistentLazySegmentTree.hpp](Persistence.md#persistentlazysegmenttree) | `build`, `add`, `prod`, `get` | 取得・更新 `O(log N)` |
| [PersistentDSU.hpp](Persistence.md#persistentdsu) | `build`, `merge`, `same`, `size` | 各操作 `O(log^2 N)` |
| [PersistentBinaryTrie.hpp](Persistence.md#persistentbinarytrie) | `insert`, `erase`, `kth_xor` | 1操作 `O(BITS)` |
| [SegmentTree2D.hpp](SegmentTree2D.md) | `set`, `prod` | `O(log H log W)` |
| [LazySegmentTree2D.hpp](LazySegmentTree2D.md) | `add`, `sum` | `O(log H log W)` |
| [WaveletMatrix.hpp](WaveletMatrix.md) | `kth_smallest`, `range_freq`, `prev_value`, `next_value` | 1クエリ `O(log sigma)` |
| [SegmentTreeBeats.hpp](SegmentTreeBeats.md) | `range_chmin`, `range_chmax`, `range_add`, 区間集約 | 償却 `O(log^2 N)` |
| [OfflineDynamicConnectivity.hpp](OfflineDynamicConnectivity.md) | 辺追加・削除、`same_query`, `size_query` | 全体 `O((Q log Q) log N)` |
| [CartesianTree.hpp](CartesianTree.md) | `parent`, `left`, `right`, `root` | 構築 `O(N)` |
| [SlopeTrick.hpp](SlopeTrick.md) | `add_abs`, `add_x_minus_a`, `shift`, `min_f` | 追加 `O(log N)` |

## 注意

- `SparseTable` の演算は結合的かつ冪等である必要があります。一般の結合的演算には `DisjointSparseTable` を使います。
- `WeightedDSU::merge(a,b,w)` は `potential(b)-potential(a)=w` を追加します。同一成分で矛盾する場合は `false` です。
- `IntervalSet` はすべて半開区間 `[l,r)` です。
- `LiChaoTree` の座標範囲はコンストラクタで `[low,high)` として固定します。
- `ConvexHullTrick` は最小値なら傾き降順、最大値なら傾き昇順で直線を追加します。任意順追加には `LiChaoTree` を使います。
- `LazySegmentTree2D.hpp` は長方形加算・長方形和に特化した `RangeAddRangeSum2D` です。
- IndexSetの詳細は [Indexset.md](Indexset.md) を参照してください。
- 永続データ構造のversion管理と制約は [Persistence.md](Persistence.md) を参照してください。

## 使い分け

### 動的座標

```cpp
DynamicSegmentTree<long long, Sum> seg(-INF, INF, 0);
seg.set(x, value);
auto total = seg.prod(l, r);
```

座標圧縮できるなら普通の配列セグ木の方が軽いです。オンラインで未知座標が来る場合や、範囲が `1e18` 近い場合に使います。

### 長方形クエリ

```cpp
SegmentTree2D<long long, Sum> static_grid(values, 0);
RangeAddRangeSum2D<long long> add_sum(h, w);
```

`SegmentTree2D` は一点更新・長方形積、`RangeAddRangeSum2D` は長方形加算・長方形和に使います。

## Wavelet Matrix

詳細: [WaveletMatrix.md](WaveletMatrix.md)

```cpp
WaveletMatrix<long long> wm(values);
wm.kth_smallest(l, r, k);       // 0-indexed
wm.range_freq(l, r, low, high); // low <= x < high
```

値を座標圧縮して構築する静的データ構造です。`count_less(l,r,x)`、値の個数を返す `count`、`kth_largest` も提供します。`prev_value` は `upper` 未満の最大値、`next_value` は `lower` 以上の最小値を `optional` で返します。構築は `O(N log sigma)`、メモリも `O(N log sigma)` です。

## Segment Tree Beats

詳細: [SegmentTreeBeats.md](SegmentTreeBeats.md)

`long long` 配列に対し、半開区間へ `chmin`、`chmax`、加算を行い、`range_sum`、`range_min`、`range_max` を取得します。空区間の最小・最大取得は対象外です。各変更は償却 `O(log^2 N)` です。

## Offline Dynamic Connectivity

詳細: [OfflineDynamicConnectivity.md](OfflineDynamicConnectivity.md)

操作を時系列順に `add_edge`、`remove_edge`、各種 `*_query` で登録し、最後に `run()` すると、クエリID順の `vector<long long>` を返します。`components_query` も利用できます。同じ辺の多重追加に対応し、追加されていない辺の削除は `invalid_argument` です。オンラインで答える構造ではありません。

## Cartesian Tree

詳細: [CartesianTree.md](CartesianTree.md)

既定では最小Cartesian Treeを構築します。同値要素は先に現れた要素を上側に保ちます。`std::greater<T>` を渡すと最大Cartesian Treeになります。根からの中間順巡回は元の添字順です。

## Slope Trick

詳細: [SlopeTrick.md](SlopeTrick.md)

凸な区分線形関数を管理します。初期値は `f(x)=0` です。

- `add_abs(a)`: `|x-a|` を加算
- `add_a_minus_x(a)`: `max(a-x,0)` を加算
- `add_x_minus_a(a)`: `max(x-a,0)` を加算
- `shift(l,r)`: `g(x)=min_{x-r <= y <= x-l} f(y)` へ変換
- `min_f`, `argmin`: 最小値と最小点区間を取得
- `clear_left`, `clear_right`: 左右いずれかの傾きを除去
