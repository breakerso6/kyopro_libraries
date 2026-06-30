# kyopro_libraries documentation

各ライブラリは、AtCoder Library のドキュメントに近い粒度で次を確認できるようにまとめています。

- `Include`: 提出コードで読み込むパス
- `コンストラクタ`: 構築方法と前提
- `関数`: 主要API、引数、返り値
- `計算量`: 計算量とメモリ
- `使用例`: 典型的な使い方

| category | overview |
| --- | --- |
| [algorithm](algorithm/README.md) | Mo、SMAWK、Kitamasa、Z-algorithm |
| [data_structure](data_structure/README.md) | Union-Find、区間クエリ、永続、Wavelet Matrix、CHT |
| [geometry](geometry/README.md) | 整数・実数幾何、凸包、最近点対、KD-tree |
| [graph](graph/README.md) | 最短路、MST、LowLink、Euler路、二部マッチング |
| [math](math/README.md) | 素因数分解、線形代数、Mobius、写像12相 |
| [string](string/README.md) | Aho-Corasick、Rolling Hash、回文 |
| [tree](tree/README.md) | HLD、全方位木DP、重心分解、Virtual Tree |
| [utility](utility/README.md) | 乱数などの共通部品 |

## 読み方

- 半開区間は `[l, r)` と書きます。
- グラフや木の頂点番号は基本的に `0`-indexed です。
- `assert` で守っている前提は、ドキュメント上でも「必要」として書いています。
- ACL依存のものは個別ページに明記しています。
