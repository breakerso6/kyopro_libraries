# ライブラリ高速化の性能比較

基準はコミット `4381463`、比較対象は今回の高速化を適用した実装です。公開関数の引数・戻り値と各ヘッダ先頭のGitHub URLコメントは維持しています。

Wavelet Matrix・Mo・素因数分解の追加高速化は [wavelet-performance.md](wavelet-performance.md) を参照してください。
SMAWK・Li Chao Tree・セグメント木・二部マッチングの追加高速化は [more-performance.md](more-performance.md) を参照してください。

## 実装の変更

- `RangeAddRangeSum2D`: 配列からの初期化を2次元差分と線形Fenwick構築に変更。`O(HW log H log W)` から `O(HW)`。
- `AhoCorasick`: 一致回数をfailure link上で集約し、全一致の列挙を省略。最大4出力の状態しかない場合や短い入力では直接集計する。`nodes[v].output` は従来通り継承済みのパターン一覧を保持し、構築時の出力伝播量は引き続き必要。
- `Kitamasa`: 指数を上位ビットから処理し、セットビットでの多項式積を `O(K)` のx乗算に置換。二乗は `O(K²)`。`n < K` は `O(K)` で係数を返す。
- `FormalPowerSeries`: ACL modintの積分を一括逆元計算に変更し、除算を係数数回から1回へ削減。追加メモリは `O(n)`。有理数・浮動小数点数・独自の型は従来の除算を使用する。
- `ClosestPair`: マージ用バッファを帯状領域でも再利用し、再帰ごとの確保を省く。重複点はソート直後に検出する。
- `RectangleUnionArea`: y区間の圧縮結果を左右のイベントで共有し、二分探索回数を半減。小さいイベントをソートし、交差する子だけを更新する。
- `Matrix`: 最後のビットの処理後に不要だった行列の二乗を省く。

## 測定条件

- Intel Core Ultra 9 285H、x86_64
- GCC 15.2.0、`-std=c++17 -O2`
- 決定的な入力生成。変更前と変更後を順番に実行し、各ケース5回の中央値を採用
- 入力生成は計測対象外。初期化・結果のチェックサム計算を含めるケースはベンチマークソースを参照
- 全ケースで変更前後のチェックサムが一致

速度比はこの入力・環境での測定結果です。実際の入力分布、型、CPU負荷によって変わります。

| ケース | 変更前 (ms) | 変更後 (ms) | 速度比 |
| --- | ---: | ---: | ---: |
| `range_add_sum_2d_build` | 58.600 | 3.657 | 16.02倍 |
| `aho_nested_count` | 34.866 | 0.364 | 95.67倍 |
| `aho_random_count` | 10.373 | 9.979 | 1.04倍 |
| `kitamasa_dense_bits` | 11.611 | 5.758 | 2.02倍 |
| `kitamasa_single_bit` | 6.005 | 5.740 | 1.05倍 |
| `fps_integral` | 15.347 | 1.308 | 11.73倍 |
| `fps_exp` | 10.030 | 6.633 | 1.51倍 |
| `closest_pair_random` | 35.597 | 32.890 | 1.08倍 |
| `closest_pair_collinear` | 7.952 | 7.645 | 1.04倍 |
| `closest_pair_duplicate` | 16.958 | 11.032 | 1.54倍 |
| `rectangle_union_random` | 134.965 | 88.478 | 1.53倍 |
| `rectangle_union_nested` | 94.720 | 53.349 | 1.78倍 |
| `matrix_pow_one` | 4.852 | 0.077 | 63.16倍 |

主な入力:

- 2次元構造: 512×512の初期配列
- Ahoの重複一致: `a`, `aa`, … の1000パターンと20万文字の `a`。ランダム一致: 長さ12の1万パターンと100万文字のテキスト
- 北政法: 漸化式の次数256。指数は `2^60 - 1` と `2^60`
- FPS: 積分20万係数、指数関数16384係数
- 最近点対: 20万点のランダム点・一直線・重複点
- 長方形和集合: 10万個のランダム長方形・入れ子長方形
- 行列: 192×192、指数1。最後の不要な二乗の影響が大きいケースであり、大きな指数で常に同じ速度比になるわけではない

## 再現手順

リポジトリルートで実行します。ACLが別の場所にある場合は `-Iac-library` を変更してください。

```bash
mkdir -p /tmp/kyopro-benchmark-before
git archive 4381463 libraries | tar -x -C /tmp/kyopro-benchmark-before
g++ -std=c++17 -O2 -I/tmp/kyopro-benchmark-before -Iac-library tools/benchmark_performance.cpp -o /tmp/kyopro-benchmark-before.out
g++ -std=c++17 -O2 -I. -Iac-library tools/benchmark_performance.cpp -o /tmp/kyopro-benchmark-after.out
/tmp/kyopro-benchmark-before.out
/tmp/kyopro-benchmark-after.out
```

## 正しさの検証

- 既存16テストと追加の `performance_regressions.test.cpp` がコンパイル・実行ともに成功
- 最終版で変更したライブラリに関連するテストを再実行済み
- 追加テストでは、素朴な漸化式との比較、`LLONG_MAX` の指数、空・重複パターン、短いテキスト、動的modint・可逆な小整数を扱える合成数mod・有理数・浮動小数点数、2次元構造のランダム更新、重複点、128bit面積を確認
- 展開した追加テストもinclude pathなしでコンパイル・実行成功。7ヘッダのGitHub URLコメントも残る
- 追加テストでAddressSanitizerとUBSanが成功。実行環境のptrace制約によりLeakSanitizerは `ASAN_OPTIONS=detect_leaks=0` で無効化

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wshadow -I. -Iac-library tests/performance_regressions.test.cpp -o /tmp/kyopro-performance-test
/tmp/kyopro-performance-test
```
