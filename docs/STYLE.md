# ドキュメント記法

[ACLのDSU](https://atcoder.github.io/ac-library/master/document_ja/dsu.html)、[Segtree](https://atcoder.github.io/ac-library/master/document_ja/segtree.html)、[Math](https://atcoder.github.io/ac-library/master/document_ja/math.html)の、API宣言に続けて動作・制約・計算量を示す構成を参考にします。

## APIページの基本形

````markdown
# Library

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [Library.hpp](../../libraries/category/Library.hpp)

できる操作と対象を1〜2文で説明します。依存ライブラリ・演算の要件を補足します。

## include

```cpp
#include "libraries/category/Library.hpp"
```

## コンストラクタ

```cpp
(1) Library object(int n);
(2) Library object(const vector<Value>& values);
```

- (1): 初期状態を説明します。
- (2): 初期配列との対応を説明します。

### 制約

- 実装に必要なサイズ・型・演算の条件を示します。

### 計算量

- (1): $O(n)$
- (2): $O(n)$

## operation

```cpp
Result object.operation(int l, int r) const;
```

何を更新し、何を返すかを説明します。空区間・失敗・不在時の結果も記載します。

### 制約

- $0 \leq l \leq r \leq n$

### 計算量

- $O(\log n)$

## 使用例

```cpp
// include、型と演算の定義、mainを含む実行可能な例を優先します。
```

## 注意

必要な補足だけを記載します。
````

## 記載ルール

- コンストラクタ・関数を見出しにし、宣言 → 動作・返り値 → 制約 → 計算量の順にします。
- 追加の制約がない関数には、空の制約欄を作る必要はありません。
- 同じ前提の補助APIや演算子はまとめても構いません。異なる計算量は操作名を付けて区別します。
- 複数クラスを1ページにまとめる場合は、クラスを `##`、APIを `###`、制約と計算量を `####` にします。
- オーバーロードは `(1)`、`(2)` で対応を示します。templateの既定型はtemplate宣言か本文で示し、型を使用する式には書きません。
- 計算量は数式 `$O(\log n)$`、型・変数・関数名はインラインコードで記載します。
- 演算の結合性、可換性、冪等性、単位元、predicateの単調性などは必要なAPIに記載します。
- 入力の上限は実装・型・アルゴリズムに根拠がある場合に示します。ACLの数値制約を本ライブラリの保証として転記しません。
- 括弧付き宣言はAPIの表記です。使用例は実際にコンパイルできるC++を優先し、出力例も確認します。
- 本文・見出しを変更するときは既存の相対リンクとアンカーを確認します。
- 性能レポートは条件・入力・測定値・再現手順の構成を維持します。APIの最悪計算量と実測の速度比を混同しません。

## 確認項目

実装の宣言・assert・失敗時の返り値、例の出力、相対リンク、コードフェンス、計算量の前提を確認します。ライブラリの仕様と説明が違う場合は、実装に一致する説明と具体的な差を記載します。
