# Timer

`libraries/utility/Timer.hpp` は経過時間を測るための軽量なタイマーです。焼きなまし、ビームサーチ、反復深化などで制限時間を見ながら処理する用途に向きます。

## Include

```cpp
#include "libraries/utility/Timer.hpp"
```

## コンストラクタ

```cpp
Timer timer;
Timer timer(time_limit_seconds);
```

構築時点を開始時刻として記録します。引数を渡した場合は制限時間も秒単位で設定します。内部時計は `std::chrono::steady_clock` です。

**制約**

- `time_limit_seconds` は秒単位

**計算量**

- $O(1)$

## reset

```cpp
void timer.reset();
```

開始時刻を現在時刻に更新します。設定済みの制限時間は変えません。

**計算量**

- $O(1)$

## set_time_limit / time_limit

```cpp
void timer.set_time_limit(double seconds);
double timer.time_limit() const;
```

制限時間を秒単位で設定・取得します。

**計算量**

- $O(1)$

## elapsed

```cpp
long long timer.elapsed_ns() const;
long long timer.elapsed_us() const;
long long timer.elapsed_ms() const;
double timer.elapsed() const;
double timer.elapsed_sec() const;
```

開始時刻からの経過時間を返します。

- `elapsed_ns`: ナノ秒
- `elapsed_us`: マイクロ秒
- `elapsed_ms`: ミリ秒
- `elapsed`, `elapsed_sec`: 秒

**計算量**

- $O(1)$

## expired / remaining

```cpp
bool timer.expired() const;
bool timer.expired(double seconds) const;
double timer.remaining() const;
double timer.remaining(double seconds) const;
```

`expired()` は初期化時または `set_time_limit` で設定した制限時間を過ぎたかを返します。`expired(seconds)` は経過時間が `seconds` 秒以上なら `true` を返します。

`remaining()` は設定済み制限時間までの残り秒数、`remaining(seconds)` は `seconds` 秒までの残り秒数を返します。残り秒数は負にはなりません。

**制約**

- `seconds` は秒単位

**計算量**

- $O(1)$

## progress

```cpp
double timer.progress() const;
double timer.progress_clamped() const;
```

設定済み制限時間に対して、経過時間が何倍かを返します。例えば制限時間の半分を過ぎていれば `0.5`、ちょうど制限時間なら `1.0` です。

`progress()` は制限時間を超えると `1.0` より大きい値を返します。`progress_clamped()` は最大 `1.0` に丸めます。制限時間が未設定または `0` 以下なら `0.0` を返します。

**計算量**

- $O(1)$

## ScopedTimer

```cpp
ScopedTimer scoped(label, os);
```

スコープを抜けると `os` に `label: X ms` を出力します。`label` の既定値は `"elapsed"`、`os` の既定値は `std::cerr` です。

## 使用例

```cpp
Timer timer(1.95);
while (!timer.expired()) {
    // time-limited search
    double ratio = timer.progress();
}

cerr << "elapsed = " << timer.elapsed_ms() << " ms\n";
```

## 注意

- `steady_clock` を使うため、システム時刻の変更の影響を受けません。
- 計測値は実行環境やOSスケジューリングの影響を受けます。
