#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"
#include "libraries/algorithm/BeamSearch.hpp"
#include <iostream>
#include <climits>

struct Info {
    using Key = int;
    int cost, key;
    std::optional<std::uint64_t> hash;
    bool finished;
    int turns = 1;
};
struct Step { int from, to; };
struct State {
    int pos = 0;
    int* applied;
    void apply(Step s) { assert(pos == s.from); pos = s.to; ++*applied; }
    void undo(Step s) { assert(pos == s.to); pos = s.from; }
    template<class Emit>
    void expand(const Info&, Emit emit) const {
        auto offer = [&](int to, int cost, int turns, bool done = false) {
            emit(Step{pos,to}, Info{cost,0,std::uint64_t(to),done,turns});
        };
        if (pos == 0) {
            offer(2, 9, 5);        // Loses to the later arrival at turn 5.
            offer(1, 2, 1);
            offer(1, 8, 1);        // Pruned immediately.
            offer(4, -100, INT_MAX, true); // Beyond horizon, even if finished.
        } else if (pos == 1) {
            offer(2, 1, 4);
            offer(2, 1, 4);        // Duplicate.
        } else {
            assert(pos == 2);
            offer(5, 0, 2);
        }
    }
};

template<bool Tree, class Policy>
void check(Policy policy) {
    int calls = 0;
    using Search = std::conditional_t<Tree, TreeSearch<State,Step,Info>,CopySearch<State,Step,Info>>;
    Search search({0,&calls}, {0,0,0,false});
    SearchConfig config;
    config.max_depth = 7;
    auto r = search.run(policy, config);
    assert(r.best && r.best->cost == 0 && r.best->depth == 7);
    const auto& path = r.best->actions;
    assert(path.size() == 3 && path[0].to == 1 && path[1].to == 2 && path[2].to == 5);
    assert(r.stats.expanded == 3 && r.stats.generated == 7 && calls == 2);
    config.max_expansions = 1;
    calls = 0;
    r = search.run(policy, config);
    assert(!r.best && r.partial && r.partial->depth == 5 && r.partial->cost == 9);
    assert(r.partial->actions.size() == 1 && calls == 0);
}

struct Jump {
    int pos, duration;
    // Legacy eager callback also supports variable turns.
    template<class Emit> void expand(const Info&, Emit emit) const {
        emit(Jump{pos+1,duration}, Step{pos,pos+1}, Info{0,0,0,false,duration});
    }
};

struct UnitInfo { int cost; std::optional<std::uint64_t> hash; bool finished; };
struct UnitState {
    int value;
    int* applications;
    void apply(int delta) { value += delta; ++*applications; }
    template<class Emit>
    void preview(const UnitInfo& current, Emit emit) const {
        assert(current.cost == -value);
        for (int d = 1; d <= 64; ++d)
            emit(d, UnitInfo{-value-d, std::nullopt, false});
    }
};
void unit_turn_preview() {
    int applications = 0;
    CopySearch<UnitState,int,UnitInfo> search({0,&applications}, {0,std::nullopt,false});
    SearchConfig config;
    config.max_depth = 3;
    auto r = search.run(FixedBeam{1}, config);
    assert(r.best && r.best->cost == -192 && r.best->depth == 3);
    assert(r.stats.generated == 192 && r.stats.expanded == 3 && applications == 2);
}

// Chokudai reaches a deeper goal while branch 2 is still waiting at turn 1.
struct GoalState {
    int pos = 0;
    void apply(Step step) { assert(step.from == pos); pos = step.to; }
    void undo(Step step) { assert(step.to == pos); pos = step.from; }
    template<class I, class Emit> void expand(const I&, Emit emit) const {
        auto offer = [&](int to, int cost, bool goal, int duration = 1) {
            I next{};
            next.cost = cost;
            next.finished = goal;
            if constexpr (std::is_same_v<I, Info>) next.turns = duration;
            emit(Step{pos,to}, next);
        };
        switch (pos) {
        case 0:
            if constexpr (std::is_same_v<I, Info>) {
                offer(9, -200, true, 8);
                offer(8, -999, false, 9); // Horizon alone is not a goal.
            }
            offer(1, 0, false);
            offer(2, 1, false);
            break;
        case 1: offer(3, 0, false); break;
        case 2: offer(6, 10, true); break;
        case 3: offer(7, -100, true); break;
        default: assert(false);
        }
    }
};
template<class I, bool Tree, class Policy>
void shortest(Policy policy) {
    using Search = std::conditional_t<Tree,TreeSearch<GoalState,Step,I>,CopySearch<GoalState,Step,I>>;
    Search search(GoalState{}, I{});
    SearchConfig config;
    config.max_depth = 9;
    config.stop_on_first_solution = true;
    auto r = search.run(policy, config);
    assert(r.stop_reason == SearchStopReason::solution_found && r.best->cost == 10);
    assert(r.best->depth == 2 && r.best->actions.size() == 2 && r.best->actions.back().to == 6);
    if constexpr (std::is_same_v<I, Info>) {
        config.max_expansions = 1;
        r = search.run(policy, config);
        assert(r.stop_reason == SearchStopReason::expansion_limit && r.best && r.best->depth == 8);
    }
    I goal{};
    goal.finished = true;
    config.max_depth = 0;
    r = Search(GoalState{}, goal).run(policy, config);
    assert(r.stop_reason == SearchStopReason::solution_found && r.best->depth == 0);
}

// An existing state may expose apply() and still emit an already-built
// child. Such a child must not apply its action again.
struct MixedState {
    int value = 0;
    int* applied;
    void apply(int gain) { value += gain; ++*applied; }
    template<class Emit> void expand(const UnitInfo& info, Emit emit) const {
        assert(info.cost == -value);
        if (value == 0) {
            emit(1, UnitInfo{-1,std::nullopt,false});
            emit(MixedState{2,applied}, 2, UnitInfo{-2,std::nullopt,false});
        } else emit(3, UnitInfo{-value-3,std::nullopt,true});
    }
};
void check_copy_callbacks() {
    int applied = 0;
    SearchConfig config;
    config.max_depth = 2;
    CopySearch<MixedState,int,UnitInfo> search({0,&applied}, {0,std::nullopt,false});
    auto result = search.run(FixedBeam{2}, config);
    assert(result.best && result.best->cost == -5 && applied == 1);
}

int main() {
    check_copy_callbacks();
    auto goals = [](auto policy) {
        shortest<Info,true>(policy);
        shortest<Info,false>(policy);
        shortest<UnitInfo,true>(policy);
        shortest<UnitInfo,false>(policy);
    };
    goals(FixedBeam{2});
    goals(Chokudai{FixedBeam{1},Unbounded{}});
    unit_turn_preview();
    auto both = [](auto policy) { check<false>(policy); check<true>(policy); };
    both(FixedBeam{1});
    both(KeyedBeam{1});
    both(GlobalPlusKeyedBeam{1,1});
    both(Chokudai{FixedBeam{1},FixedBeam{1}});
    both(Chokudai{KeyedBeam{1},KeyedBeam{1}});
    both(Chokudai{GlobalPlusKeyedBeam{1,1},GlobalPlusKeyedBeam{1,1}});
    SearchConfig config;
    config.max_depth = INT_MAX;
    auto sparse = [&](auto policy) {
        auto r = CopySearch<Jump,Step,Info>({0,INT_MAX-1}, {0,0,0,false}).run(policy, config);
        assert(!r.best && r.partial->depth == INT_MAX-1 && r.stats.expanded == 2);
    };
    sparse(FixedBeam{1});
    sparse(Chokudai{FixedBeam{1},Unbounded{}});
    for (int invalid : {0, -1}) {
        bool threw = false;
        try { CopySearch<Jump,Step,Info>({0,invalid}, {0,0,0,false}).run(FixedBeam{1}, config); }
        catch (const std::invalid_argument&) { threw = true; }
        assert(threw);
    }
    std::cout << "Hello World\n";
}
