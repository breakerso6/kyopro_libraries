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
    void preview(const Info&, Emit emit) const {
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

int main() {
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
