# kyopro_libraries

ICPC・競技プログラミング用の C++17 ライブラリ集です。

## Directory Layout

`libraries/` と `docs/` は同じ分野名で整理しています。

| category | contents | documentation |
| --- | --- | --- |
| `algorithm` | Mo's algorithm | [docs/algorithm](docs/algorithm/README.md) |
| `data_structure` | DSU、区間積、Trie、永続データ構造 | [docs/data_structure](docs/data_structure/README.md) |
| `geometry` | 整数・実数幾何、最近点対、KD-tree、長方形和集合 | [docs/geometry](docs/geometry/README.md) |
| `graph` | LowLink、Euler路、二部マッチング | [docs/graph](docs/graph/README.md) |
| `math` | XOR線形基底 | [docs/math](docs/math/README.md) |
| `string` | Aho-Corasick、Rolling Hash | [docs/string](docs/string/README.md) |
| `tree` | HLD、重心分解、全方位木DP | [docs/tree](docs/tree/README.md) |
| `utility` | 乱数生成器 | [docs/utility](docs/utility/README.md) |

ライブラリはリポジトリルートからのパスでincludeします。

```cpp
#include "libraries/geometry/GeometryInt.hpp"
#include "libraries/tree/HLD.hpp"
```

## Requirements

- C++17
- AtCoder Library
  - `libraries/tree/HLD_seg.hpp`
  - `libraries/tree/HLD_lseg.hpp`
  - `libraries/tree/HLD_seg_edge.hpp`
  - `libraries/tree/HLD_lseg_edge.hpp`
  - ACL includeを展開する場合

`ac-library/` はGit管理対象外です。必要な場合はリポジトリ直下へ配置してください。

## Include Expander

`tools/expand_includes.py` はローカルライブラリとACLの再帰includeを展開し、提出用の単一ファイルを生成します。標準ライブラリのincludeはそのまま残します。

```bash
tools/expand_includes.py main.cpp -o combined.cpp
tools/expand_includes.py main.cpp --stdout
tools/expand_includes.py main.cpp -I other_libs -o combined.cpp
```

## Tests

すべてのテストを実行する例です。

```bash
for source in tests/*_test.cpp; do
  name=$(basename "$source" .cpp)
  g++ -std=c++17 -O2 -Wall -Wextra -Wshadow -I. -Iac-library "$source" -o "/tmp/$name"
  "/tmp/$name"
done
```

展開後のソースもinclude pathなしでコンパイルできます。

```bash
tools/expand_includes.py tests/geometry_and_more_test.cpp -o /tmp/combined.cpp
g++ -std=c++17 -O2 -Wall -Wextra /tmp/combined.cpp -o /tmp/combined
/tmp/combined
```

`template.cpp` は提出用テンプレート、`tests/` は決定的テストとランダムテストです。
