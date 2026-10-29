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
    long long cost;
    optional<uint64_t> hash;
    bool finished = false; // T手に達したときだけ最終解になる
};

Info make_info(int pos, uint32_t seen, long long cost) {
    Info info;
    info.cost = cost;
    // マス数<=25、位置<32なので、この符号化は衝突しない。
    info.hash = (uint64_t(seen) << 5) | uint64_t(pos);
    return info;
}

using Action = char;

struct State {
    int pos = 0;
    uint32_t seen = 1U; // 左上の得点は最初に取得済み

    template<class Emit>
    void expand(const Info& current, Emit emit) const {
        int y = pos / W, x = pos % W;
        for (int d = 0; d < 5; ++d) {
            int ny = y + dy[d], nx = x + dx[d];
            if (ny < 0 || ny >= H || nx < 0 || nx >= W) continue;
            int to = ny * W + nx;
            uint32_t bit = uint32_t(1) << to;
            long long gain = (seen & bit) ? 0 : reward[to];

            State next = *this;
            next.pos = to;
            next.seen |= bit;
            auto info = make_info(to, next.seen, current.cost - gain);
            emit(std::move(next), commands[d], std::move(info));
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
    CopySearch<State, Action, Info> search(initial, initial_info);
    SearchConfig config;
    config.max_depth = T;
    config.time_limit_sec = 0.2;
    config.max_expansions = 20000;

    // 各深さで1件展開し、未展開候補は全体上位1000件を保持。
    auto policy = Chokudai(FixedBeam{1}, FixedBeam{1000});
    auto result = search.run(policy, config);
    if (!result.best) {
        // 途中解はT手未満なので、そのまま出力しない。
        // 全手を待機にすれば、時間切れでも必ず有効な出力になる。
        cout << reward[0] << '\n' << string(T, '.') << '\n';
        return 0;
    }

    cout << -result.best->cost << '\n';
    for (const auto& action : result.best->actions) cout << action;
    cout << '\n';
}
