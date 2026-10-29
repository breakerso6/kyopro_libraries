# SimulatedAnnealing（設計案・未実装）

[分野別一覧](README.md) · [共通事項](../README.md#共通事項) · [BeamSearch](BeamSearch.md)

実装: 未作成。予定ヘッダは `libraries/algorithm/SimulatedAnnealing.hpp` です。

焼きなましの時間・反復数・温度・採否判定を管理する部品です。近傍生成、差分評価、状態の更新・取り消しは問題側で自由に記述します。C++17向けのAPI案であり、本文の宣言・使用例は現在のライブラリでは実行できません。

## include（予定）

```cpp
#include "libraries/algorithm/SimulatedAnnealing.hpp"
```

時間・乱数は既存の [Timer](../utility/Timer.md)・[Random](../utility/Random.md) に合わせ、時間は秒単位とします。外部ライブラリへの依存は設けません。

## AnnealingConfig

```cpp
enum class Cooling { linear, geometric };

struct AnnealingConfig {
    optional<double> time_limit_sec;
    optional<uint64_t> max_iterations;
    double start_temp = 1.0;
    double end_temp = 0.0;
    Cooling cooling = Cooling::linear;
    uint64_t seed = 0;
};
```

- `time_limit_sec`: 構築からの時間上限。未指定なら時間で終了しません。
- `max_iterations`: `next()` が `true` を返す回数の上限。未指定なら反復数で終了しません。
- 両方指定した場合は先に達した方で終了します。0の上限では1回も反復しません。
- `start_temp`、`end_temp`: 開始・終了温度。
- `cooling`: 線形冷却または幾何冷却。
- `seed`: 採否判定用乱数のseed。近傍生成に使う乱数は問題側で別途管理します。

コスト最小化を基本とし、`delta = next_cost - current_cost` と定義します。最大化問題では、スコアをコストに変換するなどして符号を合わせてください。

### 制約

- 時間上限・反復上限の少なくとも片方を指定します。
- 時間上限・温度は有限の非負値です。
- `start_temp >= end_temp`
- 幾何冷却では `end_temp > 0`。線形冷却では両温度0も指定でき、その場合は山登り法になります。
- コスト差を実数へ変換して扱います。整数の差分計算でオーバーフローしないよう、問題側で十分に広い型を使ってください。

## コンストラクタ

```cpp
SimulatedAnnealing sa(const AnnealingConfig& config);
```

設定を保存し、採否判定用の乱数と開始時刻を初期化します。温度に問題のスコア単位を自動で合わせる機能は持ちません。

### 計算量

- 固定サイズの初期化を前提に $O(1)$。ログ乱数の表などを導入する場合の初期化時間・メモリは実装時に明記します。

## next

```cpp
bool sa.next();
```

予算が残っていれば次の反復を開始して `true` を返し、進捗率・温度を更新します。終了条件を満たした場合は `false` を返し、それ以降も `false` になります。

`next()` が `true` を返した回数を反復数とします。無効な近傍を引いて `continue` した場合も1反復です。近傍生成や評価を実行中に強制停止する機能はなく、1回の重い処理によって時間上限を超える場合があります。

### 計算量

- 時計・温度の更新を定数時間とした場合 $O(1)$。

## progress / temperature

```cpp
double sa.progress() const;
double sa.temperature() const;
```

直前の成功した `next()` で確定した進捗率・温度を返します。最初の `next()` より前は進捗率0・開始温度とします。

- 時間の進捗率は、構築からの経過時間を時間上限で割り、`[0,1]` に丸めた値です。
- 反復数の進捗率は、第 $i$ 反復を0始まりで数え、上限 $N \geq 2$ に対し $i/(N-1)$ とします。上限1では0です。
- 両予算がある場合は、両進捗率の大きい方を使います。
- 線形冷却: $T(p)=(1-p)T_0+pT_1$
- 幾何冷却: $T(p)=T_0(T_1/T_0)^p$

終了時に必ず終了温度で反復するという保証はありません。時間上限による終了では最後の反復が進捗率1未満になる場合があります。

### 計算量

- 値の取得は $O(1)$。

## accept

```cpp
bool sa.accept(double delta);
```

現在の反復の温度で遷移を採用するか判定します。`delta <= 0` は常に採用し、正の `delta` は温度 $T>0$ で確率 $\exp(-\mathit{delta}/T)$ により採用します。温度0では正の `delta` を却下します。同コストの遷移は採用します。

状態そのものは変更しません。採用時の反映・不採用時の取り消しは問題側の責任です。

### 制約

- 成功した `next()` の後、その反復中に使います。
- `delta` は有限値です。
- 1反復では高々一つの近傍を試します。同じ反復中の複数呼び出しは同じ採用閾値に対する判定であり、独立した抽選ではありません。

### 計算量

- 乱数生成・対数計算を定数時間とした場合 $O(1)$。

## delta_threshold

```cpp
double sa.delta_threshold();
```

現在の反復で許容するコスト増分の上限 $b$ を返します。`accept(delta)` と `delta <= b` は同じ判定になります。

温度 $T>0$ では $b=-T\log U$、$0<U<1$ とし、温度0では $b=0$ です。最初に必要になった時点で1回だけ抽選し、同じ反復の `accept`・`delta_threshold` は同じ値を共有します。内部乱数が0を返す可能性には実装側で対処し、`log(0)` や `0 * infinity` に依存しない設計とします。

評価の途中でコスト増分が $b$ を超えると証明できれば、残りの評価を省略して却下できます。単に途中の和が閾値を超えただけでは打ち切れません。残りに負の寄与がある場合などは、最終的な増分の下界を使って判定してください。

### 制約

- 成功した `next()` の後、その反復中に使います。
- 現在コストに閾値を足して使う場合も、両者は同じ温度・評価係数で比較する必要があります。

### 計算量

- 初回は乱数生成・対数計算、以後は値の取得。いずれも $O(1)$ とする目安です。

## 使用例（未実装APIの利用イメージ）

以下の `State`、近傍生成・評価関数は問題側で定義済みとします。Stateに統一した継承や固定の関数名を要求する仕様ではありません。

### 差分評価し、採用時だけ反映する

```cpp
AnnealingConfig config;
config.time_limit_sec = 1.9;
config.start_temp = 100.0;
config.end_temp = 0.1;
config.cooling = Cooling::geometric;
config.seed = 42;

SimulatedAnnealing sa(config);
Random rng(123); // 近傍生成用

State best = state;
auto cost = evaluate(state);
auto best_cost = cost;

while (sa.next()) {
    auto move = propose(state, rng);
    if (!move) continue;

    auto delta = evaluate_delta(state, *move);
    if (sa.accept(static_cast<double>(delta))) {
        apply(state, *move);
        cost += delta;
        if (cost < best_cost) {
            best_cost = cost;
            best = state;
        }
    }
}
output(best);
```

不採用候補では、盤面や重い補助データの更新を省けます。

### 一時変更して評価し、不採用なら戻す

```cpp
while (sa.next()) {
    auto backup = modify(state, rng);
    auto next_cost = evaluate(state);

    if (sa.accept(static_cast<double>(next_cost - cost))) {
        cost = next_cost;
        if (cost < best_cost) {
            best_cost = cost;
            best = state;
        }
    } else {
        rollback(state, backup);
    }
}
```

評価に状態変更が必要な問題向けです。途中で無効な遷移と判明した場合も、次の反復前に補助データを含めて復元してください。

### 採用閾値を使って評価を打ち切る

```cpp
while (sa.next()) {
    auto move = propose(state, rng);
    if (!move) continue;

    double bound = sa.delta_threshold();
    // 増分を最後まで計算するか、不採用が確定したらnulloptを返す。
    auto delta = evaluate_delta_bounded(state, *move, bound);
    if (!delta || !sa.accept(static_cast<double>(*delta))) continue;

    apply(state, *move);
    cost += *delta;
    if (cost < best_cost) {
        best_cost = cost;
        best = state;
    }
}
```

## 注意

- この部品はState・現在コスト・最良解を保持しません。最良解の保存方法や保存頻度は問題側で選べます。
- 最後の状態が最良とは限りません。最良解を返したい場合は使用例のように保存します。
- 複数近傍の選択、採用数・改善数などの統計、再加熱、多点探索は問題側で組み立てられます。これらを管理する高水準ラッパーは別途検討し、初版の必須要件にはしません。
- ビームの `State` と同じインターフェースに統一しません。近傍は途中状態を変更するもの・採用後だけ変更するものの両方を許します。
- 再現性のある比較には時間上限を未指定にし、反復数・両方の乱数seed・近傍選択順を固定します。同じseedだけでは、時間制限下の反復数までは固定されません。
- 時間依存の評価係数を使う場合、現在解と候補解を同じ進捗率で比較し、最良解保存用の評価と区別する必要があります。この管理は初版では問題側で行います。

## 参考

- [Nyaan: Multipoint Simulated Annealing](https://nyaannyaan.github.io/library/marathon/sa-manager.hpp.html): 問題固有処理を少数の関数で記述し、更新関数に採用閾値を渡す設計。
- [TERRY: AHC039公開コード](https://github.com/terry-u16/ahc039/blob/main/src/main.rs): 近傍の前処理・評価・採用処理・取り消しと、閾値による評価打ち切り。

本文のAPIはこれらを参考にした独自の設計案で、各資料の実装そのものの仕様ではありません。
