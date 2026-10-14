# WaveletMatrix

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [WaveletMatrix.hpp](../../libraries/data_structure/WaveletMatrix.hpp)

`libraries/data_structure/WaveletMatrix.hpp` は、静的列の順序統計を核として、XOR 順序、別 payload の集約・点更新、集約値による探索、2 次元矩形クエリを提供します。

[AtCoder Algorithm Lectures の発展記事](https://info.atcoder.jp/entry/algorithm_lectures/wavelet_matrix_advanced)で扱われる用途を、次の独立した構造に分けています。

| 構造 | 用途 |
| --- | --- |
| `WaveletMatrix<T>` | 任意の比較可能型を座標圧縮した順序統計・頻度 |
| `XorWaveletMatrix<UInt, LOG>` | 非負整数に XOR した後の順序統計 |
| `WaveletMatrixGroup<Key,S,...>` | 静的な可換群 payload の値域集約 |
| `WaveletMatrixMonoid<Key,S,...>` | 可換モノイド payload の値域集約と一点代入 |
| `WaveletMatrix2D<X,Y>` | 静的点群の矩形個数・矩形内 y 順序統計 |
| `WaveletMatrix2DGroup` / `WaveletMatrix2DMonoid` | 重み付き矩形集約 |

すべての添字区間・値区間・矩形は半開区間です。

## include

```cpp
#include "libraries/data_structure/WaveletMatrix.hpp"
```

## 共通の制約

- 列長を $N$ として $0 \leq l \leq r \leq N$、`access` / `get` / `set` の添字は $0 \leq i < N$
- `kth_*` は $0 \leq k < r-l$。矩形内の順序統計では $k$ は対象点数未満
- 値域と矩形の下端は上端以下
- key と payload、2次元座標と重みの配列長がそれぞれ一致する
- XOR 版は $1 \leq LOG \leq \mathrm{numeric\_limits<UInt>::digits}$、値と mask は使用ビット幅に収まる
- 集約演算と predicate の条件は各クラスの説明に従う


## 基本 Wavelet Matrix

```cpp
template<class T>
struct WaveletMatrix;
```

`T` はコピーと `operator<` による比較が可能であればよく、負数や文字列も利用できます。内部では座標圧縮します。

```cpp
vector<long long> a = {5, -2, 5, 1, 8, 3};
WaveletMatrix<long long> wm(a);

wm.size();                         // 6
wm.alphabet_size();                // 異なる値の個数
wm.access(1);                      // -2
wm[1];                             // -2
wm.kth_smallest(1, 5, 1);         // 1
wm.kth_largest(1, 5, 0);          // 8
wm.count_less(0, 6, 5);           // 3
wm.count_less_equal(0, 6, 5);     // 5
wm.range_freq(0, 6, 1, 6);        // 4: 1 <= x < 6
wm.range_freq(0, 6, 5);           // 3: 5 <= x
wm.count(0, 6, 5);                // 2
```

### 順位・前後要素

```cpp
wm.rank(5, r);                     // [0,r) にある 5 の個数
wm.rank(5, l, r);                  // [l,r) にある 5 の個数
wm.select(5, k);                   // k 番目の 5 の元配列上の添字
wm.prev_value(l, r, upper);        // upper 未満の最大値
wm.next_value(l, r, lower);        // lower 以上の最小値
wm.range_min(l, r);
wm.range_max(l, r);
```

`select`、`prev_value`、`next_value`、`range_min`、`range_max` は `optional` を返します。`access` は $O(1)$、`select` は $O(\log \sigma)$、それ以外のクエリは $O(\log \sigma)$ です。

構築時に元添字を値ごとに安定ソートし、圧縮値・元添字の出現リスト・圧縮codeをまとめて作ります。出現リストの追加メモリは $O(N + \sigma)$ の整数です。`select` は値を検索した後にリストを直接参照します。`count` は出現の少ない値にはリストの二分探索、それ以外には従来の段走査を使います。64-bit以下の整数型の `range_freq` では、値域幅が32以下で全体での出現も32個以下ならリストを直接数えます。広い値域・任意の比較型では従来の頻度クエリを使います。

### 任意の補助構造を接続する

```cpp
auto nodes = wm.range_segments(l, r, lower, upper);
for (auto [depth, left, right] : nodes) {
    // depth 回安定 partition した列の [left,right) を集約する
}

auto payload_at_depth = wm.reorder_each_level(payload);
```

`range_segments` は添字条件 `[l,r)` と値条件 `[lower,upper)` の積集合を $O(\log \sigma)$ 個の canonical node に分解します。`reorder_each_level` は別配列を各深さの並び順へ安定に並べ替えます。標準の集約クラスで足りない補助データ構造を各深さへ置くための拡張点です。

構築時間は $O(N \log N + N \log \sigma)$、メモリは $O(N \log \sigma)$ です。

クエリ時の一時vectorを省く場合は `for_each_range_segment(l, r, lower, upper, callback)` または `for_each_range_segment_less(l, r, upper, callback)` を使います。callbackは `RangeSegment` を受け取り、`range_segments` と同じ順番で呼ばれます。

全段のpayloadを同時に保存しない場合は `for_each_reordered_level(payload, callback)` を使います。callbackは `(depth, const vector<S>& current)` を受け取ります。`current` はcallback中だけ有効な参照で、保存する場合はコピーしてください。この処理の一時メモリは $O(N)$ です。

## XOR 順序

```cpp
template<class UInt, int LOG = numeric_limits<UInt>::digits>
struct XorWaveletMatrix;
```

`UInt` は unsigned 整数型です。すべての値と XOR mask は `[0, 2^LOG)` に収まる必要があります。

```cpp
vector<unsigned> a = {1, 7, 3, 5};
XorWaveletMatrix<unsigned, 3> wm(a);

unsigned mask = 6;
wm.kth_smallest(l, r, k, mask);    // (a[i] xor mask) の昇順 k 番目
wm.kth_largest(l, r, k, mask);
wm.min_xor(l, r, mask);            // min(a[i] xor mask)
wm.max_xor(l, r, mask);
wm.count_less(l, r, upper, mask);
wm.range_freq(l, r, low, high, mask);
wm.count(l, r, value, mask);       // a[i] xor mask == value の個数
```

`access(i)` は XOR 前の値を $O(1)$ で返します。このため元配列を $O(N)$ の追加メモリで保持します。構築は $O(N LOG)$、それ以外のクエリは $O(LOG)$ です。

座標圧縮された code に XOR しても元の値の XOR 順序にはならないため、任意型用の `WaveletMatrix` とは意図的に別クラスです。

## 静的な可換群集約

```cpp
template<class Key, class S,
         S (*op)(S,S), S (*e)(), S (*inverse)(S)>
struct WaveletMatrixGroup;
```

各 `Key[i]` に payload `S[i]` を対応させ、添字と key の両条件を満たす payload を集約します。`op` は結合的・可換で、`e` と `inverse` を持つ必要があります。

```cpp
long long op(long long a, long long b) { return a + b; }
long long e() { return 0; }
long long inverse(long long a) { return -a; }

WaveletMatrixGroup<int, long long, op, e, inverse> wm(keys, weights);
wm.prod(l, r, low, high);           // l <= i < r, low <= key[i] < high
wm.prod_less(l, r, upper);          // key[i] < upper
wm.all_prod(l, r);                  // key 条件なし
```

加法群には短い alias があります。

```cpp
WaveletMatrixSum<int, long long> wm(keys, weights);
```

構築は $O(N \log N + N \log \sigma)$、クエリは $O(\log \sigma)$、メモリは $O(N \log \sigma)$ です。

payloadの並べ替えとprefix構築は段ごとに処理し、全段分の一時payload配列を省きます。値域集約は2つの境界のprefix集約を差し引き、再帰的なnode列挙を省きます。構築済みレイアウトを再利用する `build(const WaveletMatrix<Key>& layout, const vector<S>& weights)` も利用できます。レイアウトはコピーして保持するため、元の構造とは独立です。重み付き2次元版ではこの方式で座標圧縮・段構築の重複を省きます。

### 集約値による値域探索

```cpp
auto result = wm.max_prefix_fold(l, r, [&](long long sum) {
    return sum <= budget;
});
```

異なる key を昇順に、同じ key の payload をひとまとまりとして追加します。predicate が成立する最大 prefix を返します。

- `result.distinct_count`: 受理した全体座標集合の先頭 key 数
- `result.fold`: 受理した payload の集約値
- `result.distinct_count < wm.matrix.alphabet_size()` なら `wm.matrix.values[result.distinct_count]` が最初に受理できない key

predicate は `pred(e()) == true` かつ、key bucket を追加するにつれて `true` から `false` へ高々一度だけ変わる必要があります。非負和と上限の組み合わせなどに使えます。

## 点更新可能な可換モノイド集約

```cpp
template<class Key, class S, S (*op)(S,S), S (*e)()>
struct WaveletMatrixMonoid;
```

各深さに segment tree を持ち、payload の一点代入に対応します。key は変更できません。

```cpp
long long op_max(long long a, long long b) { return max(a, b); }
long long e_max() { return LLONG_MIN; }

WaveletMatrixMonoid<int, long long, op_max, e_max> wm(keys, weights);
wm.set(index, new_weight);
wm.get(index);
wm.prod(l, r, low, high);
wm.prod_less(l, r, upper);
wm.all_prod(l, r);
wm.max_prefix_fold(l, r, predicate);
```

演算は結合的・可換で単位元を持つ必要があります。逆元は不要です。

```cpp
DynamicWaveletMatrixSum<int, long long> sum_wm(keys, weights);
WaveletMatrixMax<int, long long> max_wm(keys, weights);
```

構築は $O(N \log N + N \log \sigma)$、一点代入と値域集約は $O(\log \sigma \log N)$、メモリは $O(N \log \sigma)$ です。

構築時は元添字を段ごとに並べ替え、payloadと更新位置を同時に作ります。全段分の一時payload配列は不要です。`build(const WaveletMatrix<Key>& layout, const vector<S>& weights)` で構築済みレイアウトをコピーして利用でき、重み付き2次元版でも利用しています。値域集約はcallbackによるnode走査を使い、クエリごとの一時vectorを省きます。

## 2 次元点群

```cpp
WaveletMatrix2D<long long, long long> points(x, y);
points.rectangle_freq(left, right, down, up);
points.kth_smallest_y(left, right, k);
points.kth_smallest_y(left, right, down, up, k);
```

点を x 座標で安定ソートし、y 座標に Wavelet Matrix を構築します。矩形は `[left,right) x [down,up)` です。

重み付きの静的加算、動的最大値は次のように書けます。

```cpp
WaveletMatrix2DSum<long long, long long, long long> sum_points(x, y, weight);
auto sum = sum_points.rectangle_prod(left, right, down, up);

WaveletMatrix2DMax<long long, long long, long long> max_points(x, y, weight);
max_points.set(original_index, new_weight); // 入力時の点番号
auto maximum = max_points.rectangle_prod(left, right, down, up);
```

一般形は `WaveletMatrix2DGroup<X,Y,S,op,e,inverse>` と `WaveletMatrix2DMonoid<X,Y,S,op,e>` です。動的版の座標は固定で、更新できるのは payload のみです。

矩形個数と静的群集約は $O(\log N + \log \sigma_y)$、動的モノイド集約・更新は $O(\log \sigma_y \log N)$ です。$\sigma_y$ は異なる y 座標の個数です。

## 計算量

$\sigma$ は異なる key の個数、$\sigma_y$ は異なる y 座標の個数です。空列と1種類の列も含める場合、対数の因子はそれぞれ $\log(N+2)$、$\log(\sigma+2)$ と読みます。

| 操作 | 計算量 |
| --- | --- |
| 基本版・群版・モノイド版の構築 | $O(N\log(N+2)+N\log(\sigma+2))$ |
| 基本版の `access` | $O(1)$ |
| 基本版の順序統計・頻度、群版の値域集約・prefix 探索 | $O(\log(\sigma+2))$ |
| モノイド版の点更新・値域集約・prefix 探索 | $O(\log(\sigma+2)\log(N+2))$ |
| `reorder_each_level` と `for_each_reordered_level` | $O(N\log(\sigma+2))$ |
| XOR 版の構築 / `access` / その他のクエリ | $O(N LOG)$ / $O(1)$ / $O(LOG)$ |
| 2次元版の矩形個数・静的群集約 | $O(\log(N+2)+\log(\sigma_y+2))$ |
| 2次元モノイド版の矩形集約・点更新 | $O(\log(N+2)\log(\sigma_y+2))$ |

2次元版は x 座標の境界の二分探索も含めます。メモリは基本版・payload 版とも $O(N\log(\sigma+2))$、XOR 版は $O(N LOG)$ です。

## 使用例

```cpp
#include "libraries/data_structure/WaveletMatrix.hpp"
using namespace std;

int main() {
    WaveletMatrix<int> wm(vector<int>{5, -2, 5, 1});
    assert(wm.kth_smallest(0, 4, 1) == 1);
    assert(wm.count(0, 4, 5) == 2);
    assert(wm.select(5, 1) == 2);
}
```

## 注意

- `kth_*` には空区間を渡せません。頻度・集約クエリは空集合に対して `0` または単位元を返します。
- `WaveletMatrixGroup` と `WaveletMatrixMonoid` の演算は可換である必要があります。値域を canonical node に分解する順番は、元配列上の順番を表さないためです。
- 点更新版で変更できるのは payload だけです。key や 2 次元座標を変更する場合は再構築が必要です。
- `WaveletMatrixMax` / `WaveletMatrix2DMax` の単位元は `numeric_limits<S>::lowest()` です。
