# OfflineDynamicConnectivity

`libraries/data_structure/OfflineDynamicConnectivity.hpp` は辺の追加・削除を含む無向グラフの連結性をオフラインで処理します。時間軸セグメント木と `RollbackDSU` を使用します。

## Include

```cpp
#include "libraries/data_structure/OfflineDynamicConnectivity.hpp"
```

## Constructor

```cpp
OfflineDynamicConnectivity dc(n);
```

頂点は `[0,n)` です。構築直後のグラフに辺はありません。

## Edge Operations

```cpp
void add_edge(int u, int v);
void remove_edge(int u, int v);
```

現在時刻に無向辺 `{u,v}` を追加・削除します。呼び出し1回が時刻を1進めます。同じ端点の辺を複数追加でき、削除は最後に追加された未削除の1本を取り除きます。

存在しない辺を削除すると `invalid_argument` を送出します。

## Query Registration

```cpp
int same_query(int u, int v);
int size_query(int v);
int components_query();
```

現在時刻の状態に対するクエリを登録し、結果配列のIDを返します。

| function | result |
| --- | --- |
| `same_query` | 連結なら `1`、そうでなければ `0` |
| `size_query` | `v` の連結成分サイズ |
| `components_query` | グラフ全体の連結成分数 |

クエリ登録も時刻を1進めます。

## run

```cpp
vector<long long> answer = dc.run();
```

登録した全操作を処理し、`answer[query_id]` に結果を格納して返します。`run()` は登録内容を変更しないため、同じ操作列に対して再実行できます。

操作数を `Q` とすると、計算量は `O(Q log Q log N)`、メモリは `O(Q log Q + N)` です。

## Example

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/OfflineDynamicConnectivity.hpp"
using namespace std;

int main() {
    OfflineDynamicConnectivity dc(4);
    dc.add_edge(0, 1);
    int before = dc.same_query(0, 2);
    dc.add_edge(1, 2);
    int after = dc.same_query(0, 2);
    dc.remove_edge(0, 1);
    int size = dc.size_query(2);

    auto answer = dc.run();
    cout << answer[before] << '\n'; // 0
    cout << answer[after] << '\n';  // 1
    cout << answer[size] << '\n';   // 2
}
```

## Notes

- 無向グラフ専用です。自己ループは登録できますが連結性には影響しません。
- 結果は `run()` を呼ぶまで得られません。
- すべての操作を先に保存するため、オンライン入力で直前の答えを次の操作に使う問題には適しません。
