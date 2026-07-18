# BinaryTrie

`libraries/data_structure/BinaryTrie.hpp` は、固定ビット幅の非負整数をオンラインに管理する Binary Trie です。

| 構造 | 用途 |
| --- | --- |
| `BinaryTrie<UInt,BITS>` | 重複を許す multiset、XOR 順序統計、頻度、mex |
| `BinaryTrieMonoid<UInt,S,op,e,BITS>` | key ごとの payload と可換モノイド値域集約 |
| `BinaryTrieSum` / `BinaryTrieMax` | 加算・最大値集約の簡易 alias |

永続 version や prefix version の差集合には [`PersistentBinaryTrie`](Persistence.md#persistentbinarytrie)、静的な添字区間も同時に扱う場合は [`WaveletMatrix`](WaveletMatrix.md) を使います。

## multiset 版

```cpp
template<class UInt = unsigned,
         int BITS = numeric_limits<UInt>::digits>
struct BinaryTrie;
```

`UInt` は unsigned 整数型、`BITS` は使用する下位ビット数です。値と XOR mask は `[0,2^BITS)` に収まる必要があります。

### 構築・基本操作

```cpp
BinaryTrie<unsigned, 20> trie;
BinaryTrie<unsigned, 20> built(values);

trie.insert(value);                 // 1 個追加
trie.insert(value, occurrences);    // 複数追加
trie.erase(value);                  // 1 個削除、成功したかを返す
trie.erase(value, occurrences);     // 最大 occurrences 個削除、削除数を返す
trie.erase_all(value);              // すべて削除し、削除数を返す

trie.size();                        // 重複込み要素数
trie.distinct_size();               // 異なる値の個数
trie.empty();
trie.count(value);
trie.contains(value);
trie.clear();
trie.reserve(expected_values);
trie.allocated_nodes();
```

各変更・検索は `O(BITS)` です。削除した node は再利用・解放されないため、追加予定数が分かる場合は `reserve` を利用できます。

### 全体 XOR

```cpp
trie.xor_all(mask);
```

格納済みのすべての論理値へ XOR します。遅延 mask だけを更新するため `O(1)` です。その後の `insert`、`erase`、`count` などは XOR 後の値に対して動作します。

### XOR 順序統計

```cpp
trie.kth_xor(mask, k);              // (value xor mask) の昇順 k 番目
trie.min_xor(mask);                 // min(value xor mask)
trie.max_xor(mask);                 // max(value xor mask)
trie.argmin_xor(mask);              // 最小値を達成する格納値
trie.argmax_xor(mask);              // 最大値を達成する格納値

trie.kth_smallest(k);               // 格納値の昇順 k 番目
trie.kth_largest(k);                // 格納値の降順 k 番目
trie.min_value();
trie.max_value();
```

`k` は 0-indexed です。`min_xor` / `max_xor` は格納値そのものではなく XOR の結果を返します。格納値が必要なら `argmin_xor` / `argmax_xor` を使用します。

`min_value` と `max_value` は空なら `nullopt`、それ以外の順序統計は空でないことが必要です。各操作は `O(BITS)` です。

### 頻度・前後要素

```cpp
trie.count_less(upper, mask);       // value xor mask < upper
trie.count_less_equal(upper, mask);
trie.range_freq(lower, upper, mask);
trie.prev_value(upper, mask);       // upper 未満の最大値
trie.next_value(lower, mask);       // lower 以上の最小値

trie.lower_bound(value);            // mask = 0 の next_value
trie.upper_bound(value);            // value より大きい最小値
```

`mask` は省略すると `0` です。`prev_value`、`next_value`、`lower_bound`、`upper_bound` は条件を満たす値がなければ `nullopt` を返します。計算量は `O(BITS)` です。

### mex

```cpp
optional<unsigned> result = trie.mex(mask);
```

`value xor mask` の集合に含まれない最小値を `O(BITS)` で返します。重複は無視します。`BITS` bit の値がすべて存在する場合は `nullopt` です。

## 可換モノイド payload 版

```cpp
template<class UInt, class S,
         S (*op)(S,S), S (*e)(),
         int BITS = numeric_limits<UInt>::digits>
struct BinaryTrieMonoid;
```

各 key に payload を 1 個対応させる動的 map です。`op` は結合的・可換で、`e()` は単位元を返す必要があります。query-time XOR により子の順番が交換されるため、非可換演算には対応しません。

```cpp
long long op(long long a, long long b) { return a + b; }
long long e() { return 0; }

BinaryTrieMonoid<unsigned, long long, op, e, 20> trie;
trie.set(key, payload);             // 挿入または上書き
trie.erase(key);
trie.contains(key);
trie.get(key);                      // optional<S>
trie.at(key);                       // 存在することが必要
trie.size();                        // key 数
trie.xor_all(mask);                 // key 全体を XOR
```

コンストラクタへ `keys, payloads` を渡すこともできます。同じ key が複数回現れた場合は最後の payload で上書きします。

### 値域集約

```cpp
trie.all_prod();
trie.prod(lower, upper, mask);      // lower <= (key xor mask) < upper
trie.prod_less(upper, mask);
trie.prod_from(lower, mask);
trie.count_less(upper, mask);       // payload ではなく key 数
trie.kth_smallest(k, mask);         // key xor mask の昇順 k 番目
```

`mask` は省略できます。各操作は `O(BITS)`、`all_prod` は `O(1)` です。

加算・最大値には alias を利用できます。

```cpp
BinaryTrieSum<unsigned, long long, 30> sum_trie;
BinaryTrieMax<unsigned, long long, 30> max_trie;
```

`BinaryTrieMax` の単位元は `numeric_limits<S>::lowest()` です。

### 集約値による prefix 探索

```cpp
auto result = trie.max_prefix_fold(
    [&](long long sum) { return sum <= budget; }, mask);
```

key を `key xor mask` の昇順に追加し、predicate が成立する最大 prefix を `O(BITS)` で求めます。

- `result.first_rejected`: 最初に追加できなかった XOR 後の key。全 key を受理した場合は `nullopt`
- `result.accepted_keys`: 受理した key 数
- `result.fold`: 受理した payload の集約値

predicate は `pred(e()) == true` で、prefix を伸ばすにつれて `true` から `false` へ高々一度だけ変化する必要があります。非負和と上限などに利用できます。

## 計算量・メモリ

- `xor_all`: `O(1)`
- それ以外の主要操作: `O(BITS)`
- 1 個の新しい key が初めて通る経路: 最大 `BITS` node を確保
- 削除した node は保持される

## 注意

- `BITS` が型の bit 数より小さい場合、値と XOR mask の上位 bit は 0 である必要があります。
- `prod(lower, upper)` の区間は半開区間です。
- `BinaryTrie` は multiset、`BinaryTrieMonoid` は重複 key を上書きする map です。
- key を永続化する場合は `PersistentBinaryTrie`、元配列の添字区間を扱う場合は `WaveletMatrix` が適しています。
