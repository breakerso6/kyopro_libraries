# kyopro_lib

競技プログラミング用の C++ ライブラリ置き場です。

## Contents

- `libraries/HLD.hpp`: Heavy-Light Decomposition
- `libraries/HLD_seg.hpp`: HLD + `atcoder::segtree`
- `libraries/HLD_lseg.hpp`: HLD + `atcoder::lazy_segtree`
- `template.cpp`: 提出用 C++ テンプレート
- `tools/expand_includes.py`: `#include` したローカルライブラリと ACL を 1 ファイルに展開するツール
- `tests/`: ライブラリのランダムテスト
- `ac-library/`: AtCoder Library をローカルに配置してください（Git 管理対象外）

## Include Expander

`tools/expand_includes.py` は、提出用に C++ ソースを 1 ファイルへまとめるためのスクリプトです。

展開対象:

- `#include "libraries/HLD.hpp"` のようなローカル include
- `#include <atcoder/segtree>` のような ACL include
- ACL 内部の再帰 include

`ac-library/` は Git 管理対象外です。`tools/expand_includes.py` を使う前に、このリポジトリ直下へ AtCoder Library を配置してください。

標準ライブラリの include、例えば `#include <bits/stdc++.h>` はそのまま残します。

### Basic Usage

```bash
tools/expand_includes.py main.cpp -o combined.cpp
```

出力先を指定しない場合は `combined.cpp` に出力します。

```bash
tools/expand_includes.py main.cpp
```

標準出力に出したい場合:

```bash
tools/expand_includes.py main.cpp --stdout
```

追加の include ディレクトリを指定したい場合:

```bash
tools/expand_includes.py main.cpp -I other_libs -o combined.cpp
```

### Compile Example

```bash
tools/expand_includes.py main.cpp -o combined.cpp
g++ -std=c++17 -O2 -Wall -Wextra combined.cpp -o main
```

## Tests

HLD の基本クエリ:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -I. tests/HLD_random_test.cpp -o /tmp/hld_random_test
/tmp/hld_random_test
```

HLD + lazy segtree:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -I. tests/HLD_lseg_random_test.cpp -o /tmp/hld_lseg_random_test
/tmp/hld_lseg_random_test
```

展開後のソースも include path なしでコンパイルできます。

```bash
tools/expand_includes.py tests/HLD_lseg_random_test.cpp -o /tmp/hld_lseg_combined.cpp
g++ -std=c++17 -O2 -Wall -Wextra /tmp/hld_lseg_combined.cpp -o /tmp/hld_lseg_combined
/tmp/hld_lseg_combined
```
