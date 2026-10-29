// 問題固有部分をTODOにしたBeamSearchのひな型。
// TODOを埋めてから使ってください。
#include <bits/stdc++.h>
#include "libraries/algorithm/BeamSearch.hpp"
using namespace std;

using Cost = long long; // TODO: 評価値に合う型を指定する。

// TODO: 入力・前計算結果を置く。探索中に変更しない情報はStateと分けてもよい。

struct Action {
    // TODO: 1手の操作内容を定義する。
    // TODO: 取り消しに必要な更新前の値も保持する。
};

struct Info {
    Cost cost = 0;                     // 小さいほどよい。
    optional<uint64_t> hash = nullopt;  // nulloptなら重複除去なし。
    bool finished = false;             // 固定手数問題では通常falseのまま。

    // keyを使う選抜を選ぶ場合だけ、以下を有効にする。
    // using Key = int;
    // Key key = 0;

    // TODO: 必要なら差分評価用の値も追加する。
};

struct State {
    // TODO: 盤面・位置・残り資源など、現在状態を定義する。

    vector<Action> legal_actions() const {
        // TODO: 合法手を列挙する。
        // 木上版では、undoに必要な更新前の値も各Actionに持たせる。
        return {};
    }

    void apply(const Action& action) {
        // TODO: actionを適用して子状態へ進める。
        (void)action;
    }

    void undo(const Action& action) {
        // TODO: action適用前の状態へ完全に戻す。
        // 盤面だけでなく、補助データ・キャッシュも復元する。
        (void)action;
    }

    Info evaluate() const {
        Info info;
        // TODO: info.costを計算する。最大化なら、例えばcost = -score。
        // TODO: 重複除去する場合はinfo.hashを設定する。
        // TODO: 完了解で打ち切る問題ならinfo.finishedを設定する。
        // TODO: keyを使う場合は、この状態のinfo.keyを設定する。
        return info;
    }

    template<class Emit>
    void expand(const Info& current, Emit emit) {
        // currentには現在ノードの評価情報が渡される。
        // このひな型はevaluate()で再計算するため、参照しない。
        (void)current;
        for (const Action& action : legal_actions()) {
            apply(action);
            Info next_info = evaluate();
            undo(action); // emitより先に、親状態へ戻しておく。
            emit(action, std::move(next_info));

            // TODO: 必要なら、apply/undoせずcurrentから差分だけで
            // next_infoを計算する方式へ置き換える。
        }
    }
};

void read_input() {
    // TODO: 入力を読み、必要な前計算を行う。
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    read_input();

    State initial;
    // TODO: initialを初期状態にする。
    Info initial_info = initial.evaluate();

    TreeSearch<State, Action, Info> search(initial, initial_info);
    SearchConfig config;
    config.max_depth = 0; // TODO: 最大手数・最大深さを設定する。

    // 通常ビームは、まず時間制限なしで最終層まで進める。
    // 任意の打ち切りを使う場合だけ有効にする。
    // config.time_limit_sec = 1.9;
    // config.max_expansions = 20000;

    auto policy = FixedBeam{100}; // TODO: 幅を調整する。

    // key別にする場合はInfoのKey・keyとevaluate()も設定する。
    // auto policy = KeyedBeam{3};
    // auto policy = GlobalPlusKeyedBeam{100, 2};

    // chokudaiにする場合は、上のpolicy宣言と差し替える。
    // config.time_limit_sec = 1.9; // 時間または展開数の予算も設定する。
    // auto policy = Chokudai(FixedBeam{1}, FixedBeam{10000});
    // auto policy = Chokudai(KeyedBeam{1}, GlobalPlusKeyedBeam{1000, 20});
    // 保持上限をなくす場合は、第2引数をUnbounded{}にする。

    auto result = search.run(policy, config);
    if (result.best) {
        const auto& actions = result.best->actions;
        // TODO: actionsを問題の出力形式に変換する。
        // TODO: 必要ならresult.best->costも使う。
        (void)actions;
    } else {
        // TODO: 完了解がない場合の処理。有効な初期解などを出力する。
        // result.partialは途中解。問題の出力条件を満たすか確認すること。
    }
}
