# Data structures

| library | main API | complexity |
| --- | --- | --- |
| `BinaryTrie.hpp` | `insert`, `erase`, `count`, `min_xor`, `max_xor` | 1操作 `O(BITS)` |
| `RollbackDSU.hpp` | `merge`, `undo`, `snapshot`, `rollback` | `merge`/`undo`: `O(log N)` |
| `WeightedDSU.hpp` | `merge(a,b,w)`, `diff`, `same` | 償却 `O(alpha(N))` |
| `SparseTable.hpp` | `prod(l,r)` | 構築 `O(N log N)`、取得 `O(1)` |
| `DisjointSparseTable.hpp` | `prod(l,r)` | 構築 `O(N log N)`、取得 `O(1)` |
| `SWAG.hpp` | `push`, `pop`, `prod` | 償却 `O(1)` |
| `LiChaoTree.hpp` | `add_line(a,b)`, `min(x)` | `O(log X)` |
| `IntervalSet.hpp` | `insert`, `erase`, `contains`, `covers`, `mex` | 変更区間数に依存、検索 `O(log N)` |
| `Indexset.hpp` | `push`, `erase`, `contain`, `random` | 1操作 `O(1)` |
| `PersistentSegmentTree.hpp` | `build`, `set`, `prod`, `get` | 取得・更新 `O(log N)` |
| `PersistentLazySegmentTree.hpp` | `build`, `add`, `prod`, `get` | 取得・更新 `O(log N)` |
| `PersistentDSU.hpp` | `build`, `merge`, `same`, `size` | 各操作 `O(log^2 N)` |
| `PersistentBinaryTrie.hpp` | `insert`, `erase`, `kth_xor` | 1操作 `O(BITS)` |

## Notes

- `SparseTable` の演算は結合的かつ冪等である必要があります。一般の結合的演算には `DisjointSparseTable` を使います。
- `WeightedDSU::merge(a,b,w)` は `potential(b)-potential(a)=w` を追加します。同一成分で矛盾する場合は `false` です。
- `IntervalSet` はすべて半開区間 `[l,r)` です。
- `LiChaoTree` の座標範囲はコンストラクタで `[low,high)` として固定します。
- IndexSetの詳細は [Indexset.md](Indexset.md) を参照してください。
- 永続データ構造のversion管理と制約は [Persistence.md](Persistence.md) を参照してください。
