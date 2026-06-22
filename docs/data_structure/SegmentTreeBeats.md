# SegmentTreeBeats

`libraries/data_structure/SegmentTreeBeats.hpp` は区間 `chmin`、区間 `chmax`、区間加算を行いながら、区間和・最小値・最大値を取得するデータ構造です。

## Include

```cpp
#include "libraries/data_structure/SegmentTreeBeats.hpp"
```

## Constructor

```cpp
SegmentTreeBeats seg(values);
```

`values` は `vector<long long>` です。

- 構築: `O(N)`
- メモリ: `O(N)`

## Update API

```cpp
void range_chmin(int l, int r, long long x);
void range_chmax(int l, int r, long long x);
void range_add(int l, int r, long long x);
```

半開区間 `[l,r)` の各値 `a[i]` を次のように更新します。

| function | update |
| --- | --- |
| `range_chmin` | `a[i] = min(a[i], x)` |
| `range_chmax` | `a[i] = max(a[i], x)` |
| `range_add` | `a[i] += x` |

計算量: 償却 `O(log^2 N)`

## Query API

```cpp
long long range_sum(int l, int r);
long long range_min(int l, int r);
long long range_max(int l, int r);
int size() const;
```

`range_sum`、`range_min`、`range_max` は `[l,r)` の和、最小値、最大値を返します。`size()` は元の配列長です。

計算量: `O(log N)`

## Example

```cpp
#include <bits/stdc++.h>
#include "libraries/data_structure/SegmentTreeBeats.hpp"
using namespace std;

int main() {
    SegmentTreeBeats seg(vector<long long>{5, 1, 8, 6});

    seg.range_chmin(0, 4, 6); // {5,1,6,6}
    seg.range_add(1, 3, 2);   // {5,3,8,6}
    seg.range_chmax(0, 2, 4); // {5,4,8,6}

    cout << seg.range_sum(0, 4) << '\n'; // 23
    cout << seg.range_min(1, 4) << '\n'; // 4
    cout << seg.range_max(0, 3) << '\n'; // 8
}
```

## Notes

- すべての区間は0-indexed半開区間です。
- `range_sum` は空区間に対して `0` を返します。空区間への `range_min`、`range_max` は利用しないでください。
- 内部の和と更新値は `long long` です。積 `x * 区間長` や累積和が範囲内に収まる必要があります。
- 一般の遅延セグメント木と異なり、ノードが保持する第1・第2最小値・最大値を使って更新を枝刈りします。
