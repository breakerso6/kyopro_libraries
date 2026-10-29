// BeamSearchの利用例。ライブラリルートをinclude pathに指定してビルドします。
// ライブラリ内部は含まず、問題側のコードをすべて記載しています。
#include <bits/stdc++.h>
#include "libraries/algorithm/BeamSearch.hpp"
using namespace std;

int H, W, T;
vector<long long> reward;
constexpr int dy[] = {-1, 1, 0, 0, 0};
constexpr int dx[] = {0, 0, -1, 1, 0};
constexpr char commands[] = "UDLR.";

struct Info {
    using Key = int;
    Key key = 0; // 現在位置ごとに探索枠を確保
    long long cost;
    optional<uint64_t> hash;
    bool finished = false; // T手に達したときだけ最終解になる
};

Info make_info(int pos, uint32_t seen, long long cost) {
    Info info;
    info.key = pos;
    info.cost = cost;
    // マス数<=25、位置<32なので、この符号化は衝突しない。
    info.hash = (uint64_t(seen) << 5) | uint64_t(pos);
    return info;
}

struct Action {
    int from, to;
    bool first_visit;
    char command;
};

struct State {
    int pos = 0;
    uint32_t seen = 1U;

    void apply(const Action& action) {
        pos = action.to;
        seen |= uint32_t(1) << action.to;
    }

    void undo(const Action& action) {
        if (action.first_visit) seen &= ~(uint32_t(1) << action.to);
        pos = action.from;
    }

    template<class Emit>
    void expand(const Info& current, Emit emit) {
        int y = pos / W, x = pos % W;
        for (int d = 0; d < 5; ++d) {
            int ny = y + dy[d], nx = x + dx[d];
            if (ny < 0 || ny >= H || nx < 0 || nx >= W) continue;
            int to = ny * W + nx;
            uint32_t bit = uint32_t(1) << to;
            bool fresh = (seen & bit) == 0;
            Action action{pos, to, fresh, commands[d]};
            long long gain = fresh ? reward[to] : 0;

            // 評価情報だけ計算し、ここではStateを変更しない。
            emit(action, make_info(to, seen | bit, current.cost - gain));
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (!(cin >> H >> W >> T)) return 0;
    assert(H >= 1 && W >= 1 && H * W <= 25 && 0 <= T && T <= 100);
    reward.resize(H * W);
    for (auto& value : reward) {
        cin >> value;
        assert(0 <= value && value <= 1000000);
    }

    State initial;
    auto initial_info = make_info(initial.pos, initial.seen, -reward[0]);
    TreeSearch<State, Action, Info> search(initial, initial_info);
    SearchConfig config;
    config.max_depth = T;
    config.time_limit_sec = 0.2;
    config.max_expansions = 20000;

    // 各深さで各位置1件を展開。保持は全体1000件+各位置20件。
    auto policy = Chokudai(KeyedBeam{1}, GlobalPlusKeyedBeam{1000, 20});
    auto result = search.run(policy, config);
    if (!result.best) {
        // 途中解はT手未満なので、そのまま出力しない。
        // 全手を待機にすれば、時間切れでも必ず有効な出力になる。
        cout << reward[0] << '\n' << string(T, '.') << '\n';
        return 0;
    }

    cout << -result.best->cost << '\n';
    for (const auto& action : result.best->actions) cout << action.command;
    cout << '\n';
}
