# kyopro_libraries ドキュメント

C++17の競技プログラミング用ライブラリです。[ACL日本語ドキュメント](https://atcoder.github.io/ac-library/master/document_ja/)を参考に、APIの宣言、動作・返り値、制約、計算量、使用例の順で説明します。

## ライブラリ一覧

| 分野 | 内容 |
| --- | --- |
| [algorithm](algorithm/README.md) | Mo、SMAWK、Kitamasa、Z-algorithm |
| [data_structure](data_structure/README.md) | Union-Find、区間クエリ、永続化、Wavelet Matrix、CHT |
| [geometry](geometry/README.md) | 整数・実数幾何、凸包、最近点対、KD-tree |
| [graph](graph/README.md) | Graph、BFS、SCC、最短路、MST、LowLink、Euler路、二部マッチング |
| [math](math/README.md) | 整数演算、modint組合せ、素因数分解、線形代数、Mobius、写像12相 |
| [string](string/README.md) | Aho-Corasick、RollingHash、回文 |
| [tree](tree/README.md) | Tree、HLD、全方位木DP、重心分解、Virtual Tree |
| [utility](utility/README.md) | 乱数、時間計測、128-bit入出力 |

## 導入

リポジトリルートをinclude pathに指定します。

```bash
g++ -std=c++17 -O2 -I. main.cpp -o main
```

```cpp
#include "libraries/graph/GraphAlgorithms.hpp"
#include "libraries/tree/Tree.hpp"
```

ACL依存のライブラリでは、ACLの親ディレクトリも指定してください。依存は個別ページに記載しています。提出用の展開方法は [リポジトリREADME](../README.md#include-expander)、完全な実行例は [graph_and_tree.cpp](../examples/graph_and_tree.cpp) を参照してください。

## 共通事項

- 添字・頂点番号は、特記がなければ0-indexedです。入力ヘルパーの番号変換は個別APIの説明に従います。
- 区間は通常、半開区間 $[l,r)$ です。幾何の閉線分などの例外は個別ページに記載します。
- `vector`、`optional`、`pair`、`string` などは、宣言中では `std::` を省略します。`ll` は `long long`、`ull` は `unsigned long long` です。
- 宣言の `(1)`、`(2)` はオーバーロード番号です。実際のC++コードには番号を付けません。
- 制約を満たす入力を使ってください。制約外の動作は、返り値や例外が本文で明記されている場合を除き保証しません。
- `nullopt`、`false`、-1、空配列などの意味は各APIの説明に従います。始点・未到達・空区間を同一視しないでください。
- 数値型の演算は、個別に規定する場合を除き中間値も型の範囲に収まる必要があります。
- 自己ループ・多重辺・非連結入力の可否はグラフAPIごとに異なります。個別の制約を確認してください。

### 計算量の読み方

- $n$ / $N$ は配列長や頂点数、$m$ / $E$ は辺数など、個別ページで示すサイズです。
- 基本演算、型のコピー、比較、`op`、`e`、`mapping`、評価関数などは、特記がなければ $O(1)$ として記載します。これらが重い場合は、その呼び出し回数に応じたコストが加わります。
- 「ならし」「期待」「平均的」「最悪」は区別します。平均的な入力での目安を最悪時間の保証として扱わないでください。
- 時間計算量と保持・作業・返却メモリは区別して記載します。

### APIページの読み方

1. `include` と実装リンクで対象ヘッダを確認します。
2. コンストラクタと型・演算要件を確認します。
3. APIの宣言、動作・返り値、制約、計算量を確認します。
4. 使用例で呼び出し順と境界条件を確認します。

編集時の書式は [ドキュメント記法](STYLE.md) を参照してください。

## 性能測定

- [最初の高速化](performance.md)
- [Wavelet Matrix・Mo・素因数分解](wavelet-performance.md)
- [SMAWK・Li Chao Tree・セグメント木・二部マッチング](more-performance.md)

実測値は各レポートの入力・環境に対する結果です。APIページの計算量と合わせて確認してください。
