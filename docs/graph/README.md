# Graph

| library | purpose | complexity |
| --- | --- | --- |
| `LowLink.hpp` | 無向多重グラフの橋・関節点 | `O(V+E)` |
| `EulerianTrail.hpp` | 有向・無向Euler trailの構築 | `O(V+E)` |
| `HopcroftKarp.hpp` | 二部最大マッチング・最小頂点被覆 | `O(E sqrt(V))` |

## Includes and API

```cpp
#include "libraries/graph/LowLink.hpp"
#include "libraries/graph/EulerianTrail.hpp"
#include "libraries/graph/HopcroftKarp.hpp"
```

- `LowLink`: `add_edge(u,v)` の後に `build()`。結果は `bridges` と `articulation`。
- `EulerianTrail`: `directed(n,edges)` または `undirected(n,edges)`。不成立なら `nullopt`、成立時は頂点列と辺ID列を返します。
- `HopcroftKarp`: `add_edge(left,right)` の後に `max_matching()`。`minimum_vertex_cover()` は最大マッチング構築後に呼びます。
