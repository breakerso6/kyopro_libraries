# kyopro_libraries

[![Verify](https://github.com/breakerso6/kyopro_libraries/actions/workflows/verify.yml/badge.svg)](https://github.com/breakerso6/kyopro_libraries/actions/workflows/verify.yml)

ICPC・競技プログラミング用の C++17 ライブラリ集です。

## Directory Layout

`libraries/` と `docs/` は同じ分野名で整理しています。

| category | contents | documentation |
| --- | --- | --- |
| `algorithm` | Mo's algorithm、SMAWK、Kitamasa、Z-algorithm | [docs/algorithm](docs/algorithm/README.md) |
| `data_structure` | DSU、区間積、Wavelet Matrix、永続・動的データ構造、CHT | [docs/data_structure](docs/data_structure/README.md) |
| `geometry` | 整数・実数幾何、最近点対、KD-tree、長方形和集合 | [docs/geometry](docs/geometry/README.md) |
| `graph` | LowLink、二重連結成分、Euler路、二部マッチング、最短路、MST | [docs/graph](docs/graph/README.md) |
| `math` | 整数演算、modint組合せ、FPS、XOR線形基底、64-bit素因数分解、行列、Mobius、写像12相 | [docs/math](docs/math/README.md) |
| `string` | Aho-Corasick、Rolling Hash、回文 | [docs/string](docs/string/README.md) |
| `tree` | HLD、Virtual Tree、重心分解、全方位木DP | [docs/tree](docs/tree/README.md) |
| `utility` | 乱数生成器、時間計測 | [docs/utility](docs/utility/README.md) |

ライブラリはリポジトリルートからのパスでincludeします。

```cpp
#include "libraries/geometry/GeometryInt.hpp"
#include "libraries/tree/HLD.hpp"
```

## Requirements

- C++17
- AtCoder Library
  - `libraries/math/FormalPowerSeries.hpp`
  - `libraries/tree/HLD_seg.hpp`
  - `libraries/tree/HLD_lseg.hpp`
  - `libraries/tree/HLD_seg_edge.hpp`
  - `libraries/tree/HLD_lseg_edge.hpp`
  - ACL includeを展開する場合

`ac-library/` はGit管理対象外です。必要な場合はリポジトリ直下へ配置してください。

## Include Expander

`tools/expand_includes.py` は、提出用に C++ ソースを 1 ファイルへまとめるためのスクリプトです。

展開対象:

- `#include "libraries/HLD.hpp"` のようなローカル include
- `#include <atcoder/segtree>` のような ACL include
- ACL 内部の再帰 include

`ac-library/` は Git 管理対象外です。`tools/expand_includes.py` を使う前に、このリポジトリ直下へ AtCoder Library を配置してください。

標準ライブラリの include、例えば `#include <bits/stdc++.h>` はそのまま残します。
ACL include はデフォルトで展開します。ACL を展開せず、`#include <atcoder/all>` などをそのまま残したい場合は `--no-expand-acl` を指定してください。

### Basic Usage

```bash
tools/expand_includes.py main.cpp -o combined.cpp
tools/expand_includes.py main.cpp --stdout
tools/expand_includes.py main.cpp -I other_libs -o combined.cpp
```

ACL include を展開しない場合:

```bash
tools/expand_includes.py main.cpp --no-expand-acl -o combined.cpp
```

### Compile Example

```bash
tools/expand_includes.py main.cpp -o combined.cpp
g++ -std=c++17 -O2 -Wall -Wextra combined.cpp -o main
```

## Tests

テストは [online-judge-tools/verification-helper](https://github.com/online-judge-tools/verification-helper) で管理しています。
`tests/*.test.cpp` は AOJ の Hello World 問題をダミーとして使い、内部の決定的テスト・ランダムテストを実行します。

AtCoder Library をリポジトリ直下に配置した後、次のコマンドですべてのテストを実行できます。

```bash
python3 -m pip install -r requirements-verification.txt
oj-verify run --jobs 2
```

コンパイラとオプションは [`.verify-helper/config.toml`](.verify-helper/config.toml) に固定しています。
push・pull request 時にも GitHub Actions で同じ検証が走ります。

ツールを使わず個別に実行する場合:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wshadow -I. -Iac-library \
  tests/geometry_and_more.test.cpp -o /tmp/geometry_and_more.test
/tmp/geometry_and_more.test
```

展開後のソースもinclude pathなしでコンパイルできます。

```bash
tools/expand_includes.py tests/geometry_and_more.test.cpp -o /tmp/combined.cpp
g++ -std=c++17 -O2 -Wall -Wextra /tmp/combined.cpp -o /tmp/combined
/tmp/combined
```

`template.cpp` は提出用テンプレート、`tests/` は決定的テストとランダムテストです。
