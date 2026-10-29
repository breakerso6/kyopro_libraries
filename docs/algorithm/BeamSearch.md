# BeamSearch

[分野別一覧](README.md) · [共通事項](../README.md#共通事項)

実装: [BeamSearch.hpp](../../libraries/algorithm/BeamSearch.hpp)

状態をコピーして保持する方式と木上の差分更新で再現する方式を、それぞれ固定幅ビームサーチ・chokudaiサーチと組み合わせます。候補はkeyで分類でき、keyごとの上位選抜と、全体の上位にkeyごとの候補を追加する選抜を共通に扱います。C++17で利用でき、ACLへの依存はありません。

## 探索方式の選択

| 状態の保持 | 世代ごとに選抜（`FixedBeam` / `KeyedBeam` / `GlobalPlusKeyedBeam`） | `Chokudai` |
| --- | --- | --- |
| `CopySearch` | 各状態を保持し、各深さで上位の状態だけを残す | 各深さの未展開状態を保持し、繰り返し取り出して展開する |
| `TreeSearch` | 履歴の木を差分更新で辿り、各深さで上位の候補だけを残す | 各深さの未展開候補と祖先を保持し、展開対象の状態へ木上を移動する |

同じ保持方式なら、探索方式を変更しても問題側の `expand` を書き換えずに使える設計とします。状態が小さい場合や取り消しが難しい場合は `CopySearch`、状態が大きく軽量な前進・後退を実装できる場合は `TreeSearch` が候補になります。実際の速度はコピー量・移動回数・候補評価の重さに依存します。

keyによる選抜は、この4通りすべてで使えます。通常のビームサーチは意味上は全候補に同じkeyを付ける場合に相当しますが、`FixedBeam` はkeyの取得・分類を行わない専用経路とします。keyを分けた場合はkey数に応じて探索対象の総数が変わります。

## include

```cpp
#include "libraries/algorithm/BeamSearch.hpp"
```

時間計測は既存の [Timer](../utility/Timer.md) と同じ秒単位・`std::chrono::steady_clock` を使います。Timerヘッダ自体への依存はありません。

## 型・演算要件

```cpp
struct Action; // 操作列の出力と、TreeSearchでの前進・後退に使う
struct Info {
    using Key = int;
    Cost cost;
    Key key = 0;
    optional<uint64_t> hash;
    bool finished;
};
```

`Action` と `Info` は利用者が定義します。`Info` には上記メンバに加えて、評価の内訳や差分計算に必要な情報を保持できます。keyを使わない選抜だけで実行する場合は、`Key` と `key` を省略できます。省略した場合は意味上のkeyを共通値とみなします。

- `cost` は小さいほどよい評価値です。`Cost` は数値型で、大小比較・コピーができる必要があります。
- `key` は探索対象の選抜に使う分類です。位置・残り資源の区分・初手などを問題に応じて指定します。全候補に0を付ければ通常のビームになります。
- `hash` が `nullopt` の候補は重複除去しません。値を指定した場合は、同じ深さ・key・ハッシュの候補を同一状態とみなします。keyは候補を分類する値、hashはその分類内で状態を同一視する値です。
- `finished` は問題の実行可能解に到達したかを表します。`true` の候補は結果に記録し、それ以上展開しません。
- `finished == false` でも `max_depth` に到達した候補は最終解として扱います。固定ターン問題では、途中で `finished` を立てる必要はありません。
- `cost` は候補選抜と最終解比較の両方に使います。最終解を比較するときに実スコアを使いたい場合は、最終候補の `cost` をその値で生成してください。
- コスト・key・ハッシュは、当該候補が表す状態の値です。初期状態の深さは0で、1回の遷移で深さが1増えます。子のkeyは親と異なって構いません。

### 制約

- `State`、`Action`、`Info` はコピー構築可能とします。`TreeSearch` でも探索開始時などの初期状態保存にはコピーを許しますが、候補ごとのStateコピーは行わない設計です。
- 浮動小数点のコストは有限値を使います。評価は候補の保持中に変化しないものとします。
- keyを使う場合、`Key` はコピー・等値比較が可能で、`std::hash<Key>` が使える型とします。独自型は `std::hash` を特殊化してください。整数keyは連番でなくても構いません。keyの大小で候補の優先度は変えません。
- 同じkey・ハッシュとしてまとめる状態は、その後の合法手・遷移の意味も同一である必要があります。盤面以外に残り資源などが影響する場合は、それらもkeyまたはハッシュに含めてください。
- keyを使わない実行では、将来の遷移に影響する情報をハッシュ側にすべて含めてください。`Info::key` を定義していても通常版では参照しません。
- ハッシュ衝突は同一状態として処理されます。厳密な状態比較を自動で行う仕様ではありません。
- 各 `expand` が列挙する候補数は有限とします。複数深さ先へ飛ぶ遷移は初版の対象外です。

## CopySearch

### コンストラクタ

```cpp
CopySearch<State, Action, Info> search(const State& initial_state,
                                     const Info& initial_info);
```

初期状態と初期評価情報を保存します。各候補に状態本体を保持します。問題側に `undo` は要求しません。

#### 計算量

- 初期状態と初期評価情報のコピーに必要な時間・メモリ。

### expand

```cpp
template<class Emit>
void State::expand(const Info& current, Emit emit) const;

emit(State next_state, Action action, Info next_info);
```

現在の状態から遷移できる次状態を列挙します。`emit` に、遷移後の状態・実行した操作・遷移後の評価情報を渡してください。候補0個の状態は行き止まりとして処理します。

`emit` は受け取った候補を保持・選抜します。呼び出した側の状態・評価情報への参照を保持する仕様ではありません。大きい値は `std::move` で渡せます。

#### 制約

- `expand` は現在の状態と `current` を変更しません。
- 列挙順が同じなら、同点時の選択順も同じになる設計とします。

#### 計算量

- 問題側の候補生成時間に、候補の保存・選抜のコストが加わります。
- 状態本体のコピー・移動コストを定数時間とはみなしません。

## TreeSearch

### コンストラクタ

```cpp
TreeSearch<State, Action, Info> search(const State& initial_state,
                                     const Info& initial_info);
```

初期状態と初期評価情報を保存します。候補には操作・評価情報・履歴への参照を保持し、盤面などの状態本体は作業用State上で再現します。

#### 計算量

- 初期状態と初期評価情報のコピーに必要な時間・メモリ。

### apply / undo

```cpp
void State::apply(const Action& action);
void State::undo(const Action& action);
```

`apply` は親から子へ、`undo` は子から親へ状態を更新します。ライブラリは必要に応じて共通祖先まで戻り、別の枝へ進みます。

#### 制約

- `apply(action)` の直後の `undo(action)` は、補助配列やキャッシュを含めて元の状態を復元します。
- 同じ親状態で同じ操作を再適用すると同じ子状態になります。これらの関数内で新たに乱数を引いて遷移先を決めてはいけません。
- 復元に古い値が必要なら `Action` に保存してください。候補列挙中の一時変数への参照は使えません。

#### 計算量

- 問題側の1回の更新・復元時間に依存します。
- 次の展開対象までの木上距離を $L$ とすると、最大 $L$ 回の更新・復元が必要です。chokudaiサーチでは深い枝間の移動が増える場合があります。

### expand

```cpp
template<class Emit>
void State::expand(const Info& current, Emit emit);

emit(Action action, Info next_info);
```

現在の状態からの操作と遷移後の評価情報を列挙します。親ノードIDや履歴への参照はライブラリ側で付けます。

`Info` をStateから分離することで、評価項目・key・ハッシュを木上移動のたびに更新する必要を減らせます。keyは候補とともに保存され、作業用Stateに別途保持する義務はありません。候補評価には、次のどちらの書き方も使えます。

1. 差分だけを使って `next_info` を求め、Stateを変更せずに `emit` する。
2. 一時的に `apply` して評価し、`undo` してから戻る。

#### 制約

- `expand` の終了時にStateを呼び出し前と同じ状態へ戻します。候補を列挙しない場合も同様です。
- `current` は現在ノードの情報であり、変更しません。
- `emit` は候補を登録するだけで、問題側のStateに操作を適用しません。

#### 計算量

- 問題側の候補生成・評価時間に、候補の保存・選抜のコストが加わります。

## 探索設定・実行

### FixedBeam / KeyedBeam / GlobalPlusKeyedBeam

```cpp
struct FixedBeam { size_t width; };
struct KeyedBeam { size_t per_key_width; };
struct GlobalPlusKeyedBeam { size_t global_width, per_key_width; };
```

同じ深さの候補集合から、指定した規則で探索対象を選びます。`FixedBeam` は全体上位 `width` 個、`KeyedBeam` は各keyの上位 `per_key_width` 個を選びます。`GlobalPlusKeyedBeam` は次の順に選びます。重複除去を有効にした場合は、選抜の前に重複をまとめます。

1. 全候補の中から、コスト順で最大 `global_width` 個を選ぶ。
2. 1で選んだ候補を除き、各keyについてコスト順で最大 `per_key_width` 個を追加する。
3. 選んだ候補をコスト順に並べる。同コストなら登録順を使う。

各keyの追加枠は、全体枠の選抜後に適用します。全体枠に同じkeyが多く含まれていても、そのkeyに未選択候補があれば追加枠を使えます。同じ候補を二重には選びません。候補が少ないkeyの余った枠を、ほかのkeyへ移すことはしません。

| 指定 | 動作 |
| --- | --- |
| `FixedBeam{w}` | 全体の上位 `w` 個。keyの取得・分類なし |
| `KeyedBeam{k}` | 各keyの上位 `k` 個ずつ |
| `GlobalPlusKeyedBeam{w, k}` | 全体の上位 `w` 個に、残りの各keyの上位 `k` 個ずつを追加 |

全候補が同じkeyなら `KeyedBeam{w}` は `FixedBeam{w}` と同じ候補を選びます。この場合、`GlobalPlusKeyedBeam{w, k}` は幅 `w + k` と同じ選抜になります。通常版では、同一keyかどうかを実行時に検出するための走査も行わず、型から専用処理を選びます。実際の速度差は未検証です。

例えば、候補のkeyとコストが `A: 1,2,3`、`B: 4,5`、`C: 6` の場合は次のようになります。

| 指定 | 選ばれる候補（コスト順） |
| --- | --- |
| `KeyedBeam{1}` | `A:1, B:4, C:6` |
| `GlobalPlusKeyedBeam{2, 1}` | `A:1, A:2` を全体枠で選び、`A:3, B:4, C:6` を追加 |
| `FixedBeam{2}` | `A:1, A:2` |

#### 制約

- 各型の幅はすべて正とします。全体枠だけなら `FixedBeam`、key枠だけなら `KeyedBeam` を指定します。
- keyは候補の保持中に変えません。全体枠・key枠で同じコストと同点規則を使います。

#### 計算量

- key数を $G$、候補数を $N$ とすると、`GlobalPlusKeyedBeam{w, k}` の選択件数は最大 $\min(N, w+kG)$ です。`FixedBeam{w}` は最大 $\min(N,w)$、`KeyedBeam{k}` は最大 $\min(N,kG)$ です。
- 世代ごとの `FixedBeam` は全体の配列ヒープ、`KeyedBeam` はkeyごとの配列ヒープで管理します。`GlobalPlusKeyedBeam` とchokudaiは順序付き集合を使い、いずれも追加のたびに保持枠を超えた候補を破棄します。重複除去とkeyのハッシュ表操作を期待定数時間として、保持候補数 $K$ に対する追加・削除は期待 $O(\log(K+1))$ です。状態・操作・評価情報のコピーや比較のコストは別途加わります。
- `FixedBeam` はkey分類用のハッシュ表に挿入せず、型としても `Info::key` にアクセスしません。全体＋key別では、全体枠とkey別追加枠を別の索引で管理し、同じ候補を二重に展開しません。
- 重複候補の確認後、現在の最下位よりよくない候補を先に却下します。候補本体・履歴・Stateの所有権の移動は、保持が必要と分かってから行います。却下候補でも最良解・途中解の更新が必要な場合は、その結果用の履歴だけを保存します。

### Chokudai

```cpp
struct Unbounded {};

template<class Expand, class Frontier>
struct Chokudai {
    Expand expand_per_depth;
    Frontier frontier_limit;
    Chokudai(Expand expand_per_depth, Frontier frontier_limit);
};
```

`FixedBeam`・`KeyedBeam`・`GlobalPlusKeyedBeam` を直接 `run` に渡すと、深さごとに候補を集めて選抜するビームサーチになります。次の深さの展開は、その世代の選抜が完了してから始めます。

`Chokudai` には、同じ3種類の選抜型を「1走査で展開する候補」と「保持する候補」の規則として渡します。`Frontier` には保持上限なしの `Unbounded` も指定できます。C++17のコンストラクタからの型推論で `Chokudai(FixedBeam{1}, Unbounded{})` のように書けます。

`Chokudai` は深さごとの未展開候補を保持し、次を繰り返します。

1. 深さ0から `max_depth - 1` まで順に走査する。
2. その深さの未展開候補集合に `expand_per_depth` の選抜を適用し、選んだ候補を取り出して展開する。子の深さは1増える。
3. 次の走査でも、取り出していない候補を残して利用する。

同じ走査内で生成した子は、その先の深さで展開できます。期限切れでなく、全深さの未展開候補が空になった場合も終了します。無効化済みの候補を内部キューから除去する処理は、展開数に含めません。

各深さで選抜対象を確定してからコスト順に展開します。`FixedBeam{1}` は1走査・各深さで最大1個、`KeyedBeam{1}` は各keyから最大1個を展開します。予算切れでは、選んだ候補をすべて展開できるとは限りません。

`frontier_limit` が `Unbounded{}` なら未展開候補の保持上限はありません。選抜型を指定した場合は、各深さの候補追加・置き換え後にその選抜を適用し、選ばれなかった候補を破棄します。これは展開対象を選ぶ `expand_per_depth` とは独立した、探索結果を変える枝刈りです。

例えば `Chokudai(FixedBeam{1}, FixedBeam{10000})` は1個ずつ展開し、各深さで全体上位10000個を保持します。`Chokudai(KeyedBeam{1}, KeyedBeam{100})` は各keyから1個ずつ展開し、各key上位100個を保持します。key枠を使う場合、key数に比例して総保持数が増えます。展開済みノードや復元用の祖先の個数を制限する設定ではありません。保持制限で破棄した候補は、後の展開枠で復活しません。

#### 制約

- `expand_per_depth` は3種類の選抜型のいずれか、`frontier_limit` は3種類の選抜型または `Unbounded` とします。各幅は正とします。
- いずれかの規則でkeyを使う場合は `Info::Key`・`Info::key` が必要です。両方ともkeyを使わない場合はkeyの取得・分類を省きます。

### SearchConfig

```cpp
struct SearchConfig {
    int max_depth = 0;
    optional<double> time_limit_sec;
    optional<uint64_t> max_expansions;
    bool deduplicate = true;
    bool stop_on_first_solution = false;
    uint32_t time_check_interval = 1;
    bool tree_dfs_order = false;
};
```

- `max_depth`: 最大深さ。深さがこの値に達した候補は最終解として記録します。
- `time_limit_sec`: 1回の `run` の時間上限。未指定なら時間で終了しません。
- `max_expansions`: 展開回数の上限。未指定なら展開回数で終了しません。根の展開も1回です。
- `deduplicate`: ハッシュがある候補について重複除去するかを指定します。
- `stop_on_first_solution`: 最初の最終解（`finished == true` または最大深さへの到達）で終了します。既定は `false` です。同じ層の他の完了解も比較したい場合は無効のままにします。
- `time_check_interval`: 時計を実際に読む間隔。正の整数で、既定の1ではすべての確認箇所で読みます。大きくすると時計の読み取りを減らせますが、その間は期限超過の検出が遅れます。時間制限なしでは使いません。
- `tree_dfs_order`: 固定幅の `TreeSearch` を履歴の木に沿った順で展開し、前進・後退を減らします。既定は `false` でコスト順です。`true` は候補の生成順・同点の選択・評価時の乱数の対応を変え、結果が変わる場合があります。コピー版・chokudaiでは使いません。

時間・展開数を両方指定した場合は、先に達した方で終了します。`max_depth == 0` では根を最終解として返し、展開しません。時間上限・展開上限の0は、初期状態を登録して展開せず終了する指定です。

#### 制約

- `max_depth >= 0`
- 時間上限は有限の非負値。

### run

```cpp
(1) auto result = search.run(FixedBeam policy, const SearchConfig& config);
(2) auto result = search.run(KeyedBeam policy, const SearchConfig& config);
(3) auto result = search.run(GlobalPlusKeyedBeam policy, const SearchConfig& config);
(4) auto result = search.run(Chokudai<Expand, Frontier> policy,
                           const SearchConfig& config);
```

保存した初期状態から新しい探索を実行します。繰り返し呼び出しても前回の候補集合は引き継ぎません。探索の再開APIは初版では用意しません。

初期情報の `finished == true` なら根を最終解として記録し、展開せず終了します。各 `run` の予算は独立です。既定では、完了解を発見してもほかの候補がある間は探索を続けます。

`stop_on_first_solution == true` では最初に受け付けた最終解を保存して終了します。以後の `emit` は受け付けませんが、実行中の `expand` 自体は中断せず、問題側の `undo` などを最後まで実行させます。固定幅では最初に完了解が現れた深さで止まりますが、chokudaiで最初に見つかる解が最短とは限りません。

既定の `time_check_interval == 1` では、期限は少なくとも各 `expand` の開始前と各 `emit` の受付時に確認します。間隔を大きくした場合も最初の確認では時計を読み、その後は指定した回数の確認箇所ごとに読みます。問題側の関数実行を途中で強制停止する機能はなく、1回の重い評価などで時間上限を超える場合があります。打ち切り後も実行中の `expand` は終了まで戻し処理を行いますが、新しい候補は受け付けません。

#### 重複除去・同点

- 固定幅ビームでは、選抜中の同じ世代内で重複を除去します。
- chokudaiでは、同じ深さの未展開候補同士で除去します。展開済み状態を永久に記録する訪問済み集合は持ちません。
- keyを使う実行では、同じkey・ハッシュで低コストを残します。異なるkeyの候補はハッシュが同じでもまとめません。keyを使わない実行では、keyにアクセスせずハッシュだけで重複を判定します。どちらも同コストなら先に登録された候補を残し、key列挙順には依存しません。
- よりよい候補への置き換えで古い候補を無効化した場合、その候補は後から展開しません。
- 最終解は未展開キューへ入れず、生成時点で結果と比較します。重複除去とkey枠の対象は未完了候補です。最終解はkey別ではなく、すべてのkeyを通して最良のものを返します。

#### 計算量

展開回数を $E$、生成候補数を $Q$、最大保持候補数を $K$、State・Action・Infoのコピー・移動の総コストを $C$、木上移動の総コストを $M$、問題側の生成・評価の総コストを $F$ とします。以下はコスト比較・keyのコピー・ハッシュ操作を定数時間とし、ハッシュ表操作を期待定数時間とした計算量です。

- 固定幅探索は期待 $O((E+Q)\log(K+1)+F+C+M)$ です。各候補の登録時に選抜するため、その世代の全候補を保持してソートする処理はありません。
- chokudaiで1走査・1深さから $B$ 件を取り出すとき、全体上位だけの指定は $O(B\log(K+1))$、key別を含む指定は期待 $O(K+B\log(K+1))$ です。key別指定ではコスト順に走査して展開対象を確定します。この取り出しコストが各走査・各深さで加わります。
- 最良解と途中解は、更新時に履歴への参照だけを保存します。終了時に操作列をそれぞれ復元し、最大深さ $D$ に対して合計 $O(D)$ のコピー時間・返却メモリが加わります。
- コピー版の保持メモリには候補のState本体、木上版には作業用State・操作・評価情報・生存候補の祖先が含まれます。
- 固定幅でも操作列復元用の履歴があるため、総メモリを単純に「幅×Stateサイズ」とは見積もれません。
- key数 $G$ に対し `GlobalPlusKeyedBeam{w, k}` は最大 $w+kG$ 個を残します。key数が大きい問題ではkey枠の分だけ展開数・保持メモリ・履歴も増えます。
- chokudaiの上限なし設定では、未展開候補とその履歴が増え続ける場合があります。枝間移動のコストも含め、固定幅と同じ性能は保証しません。
- 生存候補・木上移動中の状態・結果候補が必要とする祖先は参照数で保持します。不要な履歴は反復的に解放し、領域を再利用します。保持メモリには過去の同時生存履歴数の最大値に相当する領域と、ハッシュ表の確保済み領域も含まれます。
- 固定幅では処理済み世代の索引を一括で破棄します。固定幅・chokudaiとも展開対象の作業配列を再利用します。通常の固定幅の候補は単独所有とし、その確保領域を探索中に再利用します。

## 返り値

```cpp
template<class Action, class Cost>
struct SearchPath {
    vector<Action> actions;
    Cost cost;
    int depth;
};

template<class Action, class Cost>
struct SearchResult {
    optional<SearchPath<Action, Cost>> best;
    optional<SearchPath<Action, Cost>> partial;
    SearchStats stats;
    SearchStopReason stop_reason;
};
```

`Cost` は `Info::cost` の型から推論します。`SearchStats` のメンバは `uint64_t expanded`、`generated`、`duplicates` と `double elapsed_sec` です。終了理由は `enum class SearchStopReason` で表します。

- `best`: 発見済み最終解のうち最小コストの操作列。未発見なら `nullopt`。
- `partial`: 最終解として扱わなかった候補のうち到達深さ最大、その中で最小コストの操作列。該当する候補が一つもなければ `nullopt`。根や、枝刈り前に受け付けた候補も対象です。
- `stats`: 展開数・生成候補数・重複除去数・経過秒数。生成候補数は、期限内に受け付けた `emit` の個数とします。
- `stop_reason`: `depth_limit`、`frontier_empty`、`time_limit`、`expansion_limit`、`solution_found` のいずれか。`solution_found` は `stop_on_first_solution` による終了です。

同コストの最終解は先に発見した方を残します。途中解も同深さ・同コストなら同様です。根に対応する操作列は空ですが、解の不在は空配列ではなく `nullopt` で区別します。

`depth_limit` は固定幅探索が最終世代まで処理できた場合、または `max_depth == 0` の場合です。chokudaiは一つの解が最大深さに達しても続行し、通常は候補枯渇か予算で終了します。両予算を同時に検出した場合は `time_limit` を優先します。

初期状態が完了解で `max_depth > 0` の場合は `frontier_empty` とします。固定幅探索の途中で時間切れになった場合、途中の世代で既に発見した解も返しますが、その世代の候補選抜が完了したとはみなしません。

ただし `stop_on_first_solution == true` で根が最終解なら、展開せず `solution_found` を返します。

`duplicates` は保持中の候補と同じハッシュ（key使用時は同じkeyも必要）を検出した回数で、却下と置き換えの両方を数えます。`elapsed_sec` は結果の操作列復元までの計測値で、関数を抜ける際の残り候補の解放時間は含みません。展開数の上限はノード間で確認し、最後に許可された1ノードが列挙する候補は、時間切れにならない限り最後まで受け付けます。

## 使用例

以下の `State`、`Action`、`Info` は問題側で定義済みとします。コピー版・木上版それぞれの `expand` 要件を満たす型を使ってください。

```cpp
SearchConfig config;
config.max_depth = 200;
config.time_limit_sec = 1.9;

// 各状態を保持 × 固定幅
CopySearch<CopyState, Action, Info> copy(initial_copy_state, initial_info);
// 通常版ではInfoにkeyを定義する必要もない。
auto a = copy.run(FixedBeam{100}, config);

// 各状態を保持 × chokudai
auto policy = Chokudai(FixedBeam{1}, FixedBeam{10000});
auto b = copy.run(policy, config);

// 木上差分更新 × 固定幅
TreeSearch<TreeState, Action, Info> tree(initial_tree_state, initial_info);
auto c = tree.run(FixedBeam{100}, config);

// 木上差分更新 × chokudai
auto d = tree.run(policy, config);

if (d.best) {
    output(d.best->actions);
} else if (d.partial) {
    // 途中解が有効な出力かは問題側で判断する。
    handle_partial(d.partial->actions);
}
```

### key別の選抜

`expand` が渡す子の `Info::key` に分類を入れます。例えば移動先の位置をkeyにする場合は、候補を追加する時点で `next_info.key = next_position` とします。初期情報にも対応するkeyを指定してください。探索方式を切り替えても、keyを決めるコードは共通です。

```cpp
// 各位置の上位3件ずつ。位置数が増えると総幅も増える。
auto per_key = tree.run(KeyedBeam{3}, config);

// 全体上位100件を選び、その残りから各位置の上位2件を追加。
auto mixed = tree.run(GlobalPlusKeyedBeam{100, 2}, config);

// chokudaiでも、各走査・各深さで同じ選抜規則を使える。
auto keyed = Chokudai(GlobalPlusKeyedBeam{10, 1},
                     GlobalPlusKeyedBeam{1000, 20});
auto repeated = tree.run(keyed, config);
```

反復数による比較では `time_limit_sec` を未指定にし、`max_expansions` を設定します。乱数を使う `expand` は問題側でもseedと列挙順を固定してください。

### 問題側のコード全体

[グリッド得点回収の6つの利用例](../../examples/beam_search_draft/README.md) に、入力・状態・候補生成・探索設定・出力までの独立したプログラムを置いています。通常版はkeyなし、key別版は現在位置をkeyとして使います。

問題固有部分をTODOにした [コピー版ひな型](../../examples/beam_search_draft/template_copy.cpp)・[木上版ひな型](../../examples/beam_search_draft/template_tree.cpp) もあります。選抜・探索方式の切り替えは各ひな型の `policy` のコメントを参照してください。

## 注意

- 木上版は固定幅・chokudaiとも、親への参照を持つ履歴と共通祖先への移動を使います。固定幅専用のオイラーツアー実装ではありません。
- コピー版は候補にState本体を保存しますが、祖先の履歴には操作だけを保存します。木上版は1回の `run` につき作業用Stateを1個コピーし、候補ごとにはコピーしません。
- 未指定の時間上限では、候補ごとの時計読み取りを省きます。通常版とkey別版の速度差について、一般的な性能倍率の保証はありません。
- 候補選抜を利用者が直接操作する低水準API、深さ別ビーム幅、世代スキップ、時間変化する評価係数は追加候補です。本文の初版APIには含めません。

## 参考

- [thunder_library](https://github.com/thun-c/thunder_library): 使い慣れた展開・前進・後退のインターフェース。
- [eijirou: 差分更新ビームサーチライブラリの実装](https://eijirou-kyopro.hatenablog.com/entry/2024/02/01/115639): Action・Evaluator・Stateの分離。
- [rhoo: 爆速ビームサーチライブラリを作る](https://qiita.com/rhoo/items/2f647e32f6ff2c6ee056): 候補列挙・選抜・状態管理の分離。
- [jibjib: 木上のビームサーチ：高速化編](https://trap.jp/post/2920/): 状態更新用の情報と評価候補の分離。紹介されている実装は世代スキップなしに特化しています。

本文のAPIはこれらを参考にした独自の実装で、各資料の実装そのものの仕様ではありません。
