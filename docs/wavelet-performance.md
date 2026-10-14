# Wavelet Matrix・Mo・素因数分解の高速化

比較する3ヘッダの基準実装はコミット `4381463` と同一です。前回の高速化は [performance.md](performance.md) を参照してください。

## Wavelet Matrix

- 元添字を値順に安定ソートし、圧縮値・元添字の出現リスト・元配列の圧縮codeをまとめて構築。ソート後に全要素を二分探索する処理を省く。
- `access` は既存の圧縮codeを直接参照して `O(log sigma)` から `O(1)` へ。XOR版も元配列を保持して `O(LOG)` から `O(1)` へ。
- `select` は出現リストを直接参照し、`O(log sigma log N)` から `O(log sigma)` へ。
- `count` は出現回数が少ない値では出現リストを二分探索し、頻出値では従来の段走査を使用する。全区間の個数はリストの長さから返す。
- 64-bit以下の整数型では、値域幅が32以下かつ全体の出現も32個以下の `range_freq` を短いリスト走査で集計する。広い値域や比較型は従来の頻度クエリを使う。
- 静的群集約の `prod` / `prod_less` は境界に沿ったprefix集約を使用し、再帰的なnode列挙と一時vectorを省く。モノイド版はcallbackによるnode走査を使用し、一時vectorを省く。
- 重み付き版はpayloadを段ごとに並べ替えて補助構造を構築し、全段分の一時payload配列を省く。点更新位置も元添字の並べ替えと同時に作る。
- 重み付き2次元版は、点群の構築済みWavelet Matrixをコピーして再利用し、同じy列の圧縮・段構築をもう一度行う処理を省く。

`prefix_one`、`original_code`、`children`、`range_segments`、`reorder_each_level` の形式・並び順は維持している。追加APIと計算量は [WaveletMatrix.md](data_structure/WaveletMatrix.md) を参照。

基本版には整数 `N + sigma + 1` 個分の出現リストを追加し、XOR版には元配列1本を追加する。永続的なメモリはその分増えるが、重み付き版の構築時ピークは全段の一時配列を省くことで減る。表はプロセス全体の実測値で、最終的に保持する構造だけのサイズとは異なる。

## その他

- `Mo`: ブロック番号を先に計算し、ソートの比較ごとの2回の整数除算を省く。並べ替え規則は同じ。追加メモリは `O(Q)`。
- `Factorization` (C++): Pollard's RhoをBrent法と128個の差分ごとのgcd集約に変更。多項式の定数加算で2回目の剰余除算を省く。まとめたgcdが `n` になった場合の個別再検査・再試行も実装する。

## 測定結果

Intel Core Ultra 9 285H、GCC 15.2.0、`-std=c++17 -O2`。変更前後を順番に実行し、各ケース5回の中央値を採用。入力生成は計測対象外で、全ケースのチェックサムが一致した。

| ケース | 変更前 (ms) | 変更後 (ms) | 速度比 |
| --- | ---: | ---: | ---: |
| `wm_build_large_alphabet` | 40.418 | 32.128 | 1.26倍 |
| `wm_build_small_alphabet` | 6.168 | 5.939 | 1.04倍 |
| `wm_access` | 206.689 | 0.541 | 381.84倍 |
| `wm_select` | 82.054 | 3.581 | 22.92倍 |
| `wm_count_sparse` | 16.375 | 4.665 | 3.51倍 |
| `wm_count_dense` | 1.026 | 0.592 | 1.73倍 |
| `wm_range_freq_narrow` | 25.105 | 6.815 | 3.68倍 |
| `wm_range_freq_wide` | 26.154 | 28.149 | 0.93倍 |
| `wm_kth` | 4.486 | 4.182 | 1.07倍 |
| `xor_wm_access` | 256.525 | 0.255 | 1005.15倍 |
| `wm_sum_build` | 78.619 | 56.289 | 1.40倍 |
| `wm_sum_prod` | 99.282 | 92.886 | 1.07倍 |
| `wm_dynamic_sum_build` | 116.380 | 95.009 | 1.22倍 |
| `wm_2d_sum_build` | 121.931 | 77.098 | 1.58倍 |
| `mo_sort_noop` | 4.262 | 3.332 | 1.28倍 |
| `mo_sum` | 12.755 | 10.990 | 1.16倍 |
| `factorization_semiprimes` | 79.929 | 6.480 | 12.33倍 |

| 構築時ピークRSS | 変更前 (MiB) | 変更後 (MiB) | 減少率 |
| --- | ---: | ---: | ---: |
| 静的群集約 | 78.3 | 53.0 | 32.3% |
| 点更新モノイド集約 | 137.4 | 113.7 | 17.2% |

入力・測定範囲:

- Wavelet Matrix: 20万要素、乱数で10万値または4値から生成
- `access`: 元添字をランダムに50万回参照
- `select`, `count`, `range_freq`, `kth`, 静的群集約: 5万クエリ。狭い頻度クエリは値域幅3
- XOR版: `unsigned`, `LOG=20`
- 重み付き構築: 20万要素、`long long` payload。2次元版はx座標もランダム
- Mo: 同じ5万区間。`mo_sort_noop` は追加・削除が空のcallback、`mo_sum` は区間和を更新
- 素因数分解: 40億付近の異なる素数2個の積を10個分解。入力は固定だが探索には乱数を使うため時間が変動する
- RSS: 同じ入力の構築1回を新しいプロセスで測定。入力配列、構築用バッファ、保持する構造をすべて含む

速度比はこの環境と入力に限る。大きな `access` の速度比は、各段の依存するメモリアクセスがなくなった結果で、他のクエリも同じ割合で速くなることを意味しない。広い値域の頻度クエリはこの測定で約8%時間が増えており、全ケースでの高速化を保証するものではない。

## 再測定

リポジトリルートで実行する。このベンチマークにはACLは不要。

```bash
mkdir -p /tmp/kyopro-wavelet-baseline
git archive 4381463 libraries | tar -x -C /tmp/kyopro-wavelet-baseline
g++ -std=c++17 -O2 -I/tmp/kyopro-wavelet-baseline tools/benchmark_wavelet.cpp -o /tmp/kyopro-wavelet-before.out
g++ -std=c++17 -O2 -I. tools/benchmark_wavelet.cpp -o /tmp/kyopro-wavelet-after.out
/tmp/kyopro-wavelet-before.out
/tmp/kyopro-wavelet-after.out
/usr/bin/time -f 'peak_rss_kib=%M' /tmp/kyopro-wavelet-before.out memory-group
/usr/bin/time -f 'peak_rss_kib=%M' /tmp/kyopro-wavelet-after.out memory-group
/usr/bin/time -f 'peak_rss_kib=%M' /tmp/kyopro-wavelet-before.out memory-monoid
/usr/bin/time -f 'peak_rss_kib=%M' /tmp/kyopro-wavelet-after.out memory-monoid
```

## 正しさの検証

関連する既存3テストと追加の `wavelet_performance_regressions.test.cpp` をコンパイル・実行。追加テストでは以下を確認する。

- 空配列・単一値・重複値・異なるalphabetサイズでの再構築、すべてのselect、存在しない値・不正な順位
- ランダム頻度クエリと素朴な配列走査の比較、64-bit符号付き・符号なしの端点
- `operator<` だけを持つ型、構造自身の `values` を入力にした再構築
- 段ごとの安定partitionとstreaming構築の対応、構築済みレイアウトのコピーと元構造からの独立性
- 静的群・点更新モノイドの集約、XOR群、XOR版の64-bit上位ビット
- 素朴な素因数分解との1000入力比較、小さい平方数の反復、64-bit最大値・大きな素数・大きな素数の平方。因子の素数性と128-bitでの積を確認
- 空区間を含むMoの1000クエリ、再実行・クエリなしの入力

追加テストは展開後のソースでも実行し、3ヘッダのGitHub URLコメントが残ることを確認する。AddressSanitizerとUBSanでも検証する。環境のptrace制約により、LeakSanitizerのみ `ASAN_OPTIONS=detect_leaks=0` で無効化する。
