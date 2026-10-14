# Int128IO

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [Int128IO.hpp](../../libraries/utility/Int128IO.hpp)

`libraries/utility/Int128IO.hpp` は `__int128_t` と `unsigned __int128` の10進入出力を追加します。

## include

```cpp
#include "libraries/utility/Int128IO.hpp"
```

## 共通の制約

- GCC/Clang の128bit整数拡張が利用可能な環境
- 符号付き入力は $-2^{127} \leq x < 2^{127}$、符号なし入力は $0 \leq x < 2^{128}$

不正な10進文字列や範囲外の入力では `failbit` が立ち、読み込み先の値は変わりません。符号付き入力は先頭の `+` / `-`、符号なし入力は先頭の `+` に対応します。


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

## 計算量

- 10進表記の桁数を $d$ として入出力とも $O(d)$
- 出力時の一時メモリは $O(d)$

## 使用例

```cpp
#include "libraries/utility/Int128IO.hpp"
using namespace std;

int main() {
    istringstream input("170141183460469231731687303715884105727");
    __int128_t value = 0;
    input >> value;
    assert(!input.fail());
    ostringstream output;
    output << value;
    assert(output.str() == "170141183460469231731687303715884105727");
}
```

## 注意

- 10進表記のみ対応します。
- 入力値が範囲外の場合は `failbit` が立ちます。
- `__int128_t` は GCC/Clang の拡張です。
