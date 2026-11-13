# AtCoder 提出欄でのライブラリ展開

[ユーザースクリプト本体](../tools/atcoder-library-expander.user.js) を Tampermonkey に登録すると、AtCoder の提出ページと問題ページの提出欄に「ライブラリを展開」「元に戻す」が追加されます。

## インストール

1. ブラウザに [Tampermonkey](https://www.tampermonkey.net/) をインストールします。
2. Tampermonkey の「新規スクリプトを追加」を開き、既定の内容をすべて削除して `tools/atcoder-library-expander.user.js` の全文を貼り付け、保存します。
3. AtCoder にログインして提出ページまたは問題ページを再読み込みします。

GitHub にこのファイルを push した後は、GitHub 上のファイルの Raw ボタンから直接インストールすることもできます。
GitHub へのアクセスを拡張機能に確認された場合は、`raw.githubusercontent.com` を許可してください。
コードの取得には Tampermonkey の [`GM_xmlhttpRequest`](https://www.tampermonkey.net/documentation.php?q=GM_xmlhttpRequest) を使用します。

## 使い方

提出欄に通常どおりコードを書きます。

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/Compress.hpp"
using namespace std;

int main() {
    vector<long long> xs{100, -5, 100, 7};
    Compress<long long> comp(xs);
    for (int i : comp.compressed(xs)) cout << i << ' ';
    cout << '\n';
}
```

「ライブラリを展開」を押すと、必要なヘッダを GitHub から取得して提出欄の include を置換します。ライブラリ内部の依存関係も再帰的に展開し、重複定義を防ぎます。
`"data_structure/Compress.hpp"` や `"kyopro_libraries/libraries/data_structure/Compress.hpp"` も使用できます。
ヘッダ内部では、リポジトリルートからのパスと、そのヘッダからの相対パスを解決します。標準ライブラリの `<bits/stdc++.h>` などはそのまま残します。

展開後は内容を確認し、通常の提出ボタンで提出してください。このスクリプトが自動で提出することはありません。
「元に戻す」で直前の展開前のコードへ戻せます。展開後に編集した場合は、破棄する前に確認が表示されます。復元用のコードはページを閉じると失われます。

取得失敗時は元のコードを保持してエラーを表示します。取得中にコードを編集した場合も上書きを中止します。
提出コードを GitHub へ送信する処理はなく、取得するヘッダのパスだけを GET リクエストに使います。

## 取得設定

「GitHub の取得設定」で次の値を変更できます。設定はユーザースクリプトのストレージに保存され、次回も使用されます。

| 項目 | 初期値 |
| --- | --- |
| Owner | `breakerso6` |
| Repository | `kyopro_libraries` |
| ブランチ・タグ・コミット | `feature/icpc-extra-libraries` |
| ACL も展開する | オフ |

参照先を固定したい場合は、ブランチ名の代わりにコミット SHA を指定してください。公開リポジトリを対象としており、GitHub のトークンは不要です。

ACL の include も展開したい場合はチェックを入れます。`atcoder/ac-library` の `master` ブランチから取得します。
オフの場合は `<atcoder/all>` などをそのまま残します。

## 対応範囲

- AtCoder の Ace エディタ、通常のテキスト欄、従来の CodeMirror 形式に対応します。
- 通常の `#include "..."` と、`<libraries/...>`・`<分野名/...>`、オプションで `<atcoder/...>` を展開します。
- マクロ形式の include、ローカルの絶対パス、GitHub に存在しない手元のファイルは対象外です。
- C++ プリプロセッサの条件式は評価しません。`#if` 内のヘッダも取得するため、無効な条件内の include も GitHub 上に存在する必要があります。条件分岐自体は展開結果に保持します。
- ヘッダは通常の include guard または `#pragma once` を用いる形式を想定します。複数回の include で動作を変える X-macro 用ヘッダには対応しません。
- 取得内容のキャッシュは1回の展開中だけ保持します。次の展開では再取得します。

## 検証

追加の npm パッケージなしでテストできます。Node.js 22 以降を使用してください。

```bash
node --test tests/atcoder_library_expander.test.cjs
```

実際の提出欄での確認は、AtCoder にログインしたブラウザで行ってください。
