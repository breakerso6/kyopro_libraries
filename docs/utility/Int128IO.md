# Int128IO

`libraries/utility/Int128IO.hpp` は `__int128_t` と `unsigned __int128` の10進入出力を追加します。

## Include

```cpp
#include "libraries/utility/Int128IO.hpp"
```

## 入出力

```cpp
__int128_t x;
cin >> x;
cout << x << '\n';

unsigned __int128 y;
cin >> y;
cout << y << '\n';
```

通常の整数型と同じように `istream` / `ostream` の `operator>>`, `operator<<` が使えます。

## 注意

- 10進表記のみ対応します。
- 入力値が範囲外の場合は `failbit` が立ちます。
- `__int128_t` は GCC/Clang の拡張です。
