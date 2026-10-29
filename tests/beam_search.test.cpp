#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"
#include <bits/stdc++.h>
#include "libraries/algorithm/BeamSearch.hpp"
#include "libraries/algorithm/BeamSearch.hpp"
using namespace std;

struct Action {
    int from, to;
    bool operator==(const Action& other) const { return from == other.from && to == other.to; }
};
struct Info {
    using Key = int;
    long long cost;
    int key;
    optional<uint64_t> hash;
    bool finished;
};
struct Edge { int to, weight, key; };
struct Problem {
    vector<vector<Edge>> edges;
    vector<bool> finished, hashable;
    vector<uint64_t> hash_values;
    optional<uint64_t> hash(int vertex) const {
        if (!hashable[vertex]) return nullopt;
        return hash_values.empty() ? uint64_t(vertex) : hash_values[vertex];
    }
    Info initial() const { return {0, 0, hash(0), finished[0]}; }
    Info next(const Info& current, Edge e) const {
        return {current.cost + e.weight, e.key,
                hash(e.to), finished[e.to]};
    }
};

struct CopyState {
    const Problem* problem;
    vector<int> path{0};
    template<class Emit> void expand(const Info& current, Emit emit) const {
        for (auto e : problem->edges[path.back()]) {
            Action action{path.back(), e.to};
            auto next = *this;
            next.path.push_back(e.to);
            emit(std::move(next), action, problem->next(current, e));
        }
    }
};
struct TreeState {
    static int copies, applies, undoes;
    const Problem* problem;
    vector<int> path{0};
    explicit TreeState(const Problem* p) : problem(p) {}
    TreeState(const TreeState& other) : problem(other.problem), path(other.path) { ++copies; }
    void apply(const Action& action) {
        ++applies;
        assert(path.back() == action.from);
        path.push_back(action.to);
    }
    void undo(const Action& action) {
        ++undoes;
        assert(path.size() > 1 && path.back() == action.to);
        path.pop_back();
        assert(path.back() == action.from);
    }
    template<class Emit> void expand(const Info& current, Emit emit) {
        auto edges = problem->edges[path.back()];
        for (auto e : edges) {
            Action action{path.back(), e.to};
            apply(action);
            Info info = problem->next(current, e);
            undo(action);
            emit(action, info);
        }
    }
};
int TreeState::copies = 0;
int TreeState::applies = 0;
int TreeState::undoes = 0;

// Independent, deliberately simple oracle: materialize vectors, sort them,
// deduplicate by linear search, and copy complete paths. No library internals.
struct Rule { size_t global, per_key; };
Rule rule(FixedBeam p) { return {p.width, 0}; }
Rule rule(KeyedBeam p) { return {0, p.per_key_width}; }
Rule rule(GlobalPlusKeyedBeam p) { return {p.global_width, p.per_key_width}; }
Rule rule(Unbounded) { return {numeric_limits<size_t>::max(), 0}; }
struct RefNode { int vertex; Info info; vector<Action> path; uint64_t order; };
using Result = SearchResult<Action, long long>;

vector<RefNode> select_nodes(vector<RefNode> values, Rule selection) {
    stable_sort(values.begin(), values.end(), [](const auto& a, const auto& b) {
        return tie(a.info.cost, a.order) < tie(b.info.cost, b.order);
    });
    vector<RefNode> selected;
    map<int, size_t> counts;
    size_t global = 0;
    for (auto& value : values) {
        if (global < selection.global) { ++global; selected.push_back(value); }
        else if (counts[value.info.key] < selection.per_key) {
            ++counts[value.info.key]; selected.push_back(value);
        }
    }
    return selected;
}

Result reference(const Problem& problem, SearchConfig config, Rule expansion,
                 optional<Rule> retention, bool keyed) {
    Result result;
    uint64_t order = 0;
    vector<vector<RefNode>> queues(config.max_depth + 1);
    auto consider = [&](const RefNode& node) {
        int depth = static_cast<int>(node.path.size());
        bool terminal = node.info.finished || depth == config.max_depth;
        auto& target = terminal ? result.best : result.partial;
        if (!target || (!terminal && depth > target->depth) ||
            ((terminal || depth == target->depth) && node.info.cost < target->cost))
            target = SearchPath<Action, long long>{node.path, node.info.cost, depth};
        return terminal;
    };
    auto push = [&](RefNode node, Rule cap) {
        auto& queue = queues[node.path.size()];
        if (config.deduplicate && node.info.hash) {
            auto it = find_if(queue.begin(), queue.end(), [&](const auto& old) {
                return old.info.hash == node.info.hash && (!keyed || old.info.key == node.info.key);
            });
            if (it != queue.end()) {
                ++result.stats.duplicates;
                if (it->info.cost <= node.info.cost) return;
                queue.erase(it);
            }
        }
        queue.push_back(std::move(node));
        queue = select_nodes(std::move(queue), cap);
    };
    RefNode root{0, problem.initial(), {}, order++};
    if (consider(root)) {
        result.stop_reason = config.max_depth == 0 ? SearchStopReason::depth_limit : SearchStopReason::frontier_empty;
        return result;
    }
    push(root, retention.value_or(expansion));
    auto expand = [&](const RefNode& node) {
        ++result.stats.expanded;
        for (auto e : problem.edges[node.vertex]) {
            auto path = node.path;
            path.push_back({node.vertex, e.to});
            RefNode child{e.to, problem.next(node.info, e), std::move(path), order++};
            ++result.stats.generated;
            if (!consider(child)) push(std::move(child), retention.value_or(expansion));
        }
    };
    auto budget = [&] {
        if (config.max_expansions && result.stats.expanded >= *config.max_expansions) {
            result.stop_reason = SearchStopReason::expansion_limit;
            return false;
        }
        return true;
    };
    if (!retention) {
        for (int depth = 0; depth < config.max_depth; ++depth) {
            while (!queues[depth].empty()) {
                if (!budget()) return result;
                auto node = queues[depth].front();
                queues[depth].erase(queues[depth].begin());
                expand(node);
            }
            if (depth == config.max_depth - 1) {
                result.stop_reason = SearchStopReason::depth_limit; return result;
            }
            if (queues[depth + 1].empty()) return result;
        }
    } else {
        for (;;) {
            for (int depth = 0; depth < config.max_depth; ++depth) {
                if (queues[depth].empty()) continue;
                if (!budget()) return result;
                auto selected = select_nodes(queues[depth], expansion);
                for (const auto& node : selected) {
                    auto& queue = queues[depth];
                    queue.erase(find_if(queue.begin(), queue.end(), [&](const auto& x) { return x.order == node.order; }));
                }
                for (const auto& node : selected) {
                    if (!budget()) return result;
                    expand(node);
                }
            }
            bool empty = true;
            for (const auto& queue : queues) if (!queue.empty()) empty = false;
            if (empty) return result;
        }
    }
    return result;
}

void equal_result(const Result& a, const Result& b) {
    auto compare_path = [](const auto& x, const auto& y) {
        assert(bool(x) == bool(y));
        if (x) { assert(x->cost == y->cost); assert(x->depth == y->depth); assert(x->actions == y->actions); }
    };
    compare_path(a.best, b.best);
    compare_path(a.partial, b.partial);
    assert(a.stop_reason == b.stop_reason);
    assert(a.stats.expanded == b.stats.expanded);
    assert(a.stats.generated == b.stats.generated);
    assert(a.stats.duplicates == b.stats.duplicates);
    assert(a.stats.elapsed_sec >= 0);
}

template<class Policy>
void check(const Problem& problem, const SearchConfig& config, Policy policy, Rule selection,
           optional<Rule> retention, bool keyed) {
    auto expected = reference(problem, config, selection, retention, keyed);
    CopyState copy{&problem};
    CopySearch<CopyState, Action, Info> a(copy, problem.initial());
    equal_result(a.run(policy, config), expected);
    TreeState tree(&problem);
    TreeState::copies = 0;
    TreeSearch<TreeState, Action, Info> b(tree, problem.initial());
    equal_result(b.run(policy, config), expected);
    assert(TreeState::copies == 2); // saved initial state + one work state, independent of Q
    equal_result(b.run(policy, config), expected);
    assert(TreeState::copies == 3);
    assert(tree.path == vector<int>{0});
}

void test_oracle() {
    mt19937 rng(780923);
    for (int trial = 0; trial < 45; ++trial) {
        Problem problem;
        int n = 8;
        problem.edges.resize(n); problem.finished.resize(n); problem.hashable.resize(n);
        for (int v = 0; v < n; ++v) {
            problem.finished[v] = rng() % 9 == 0;
            problem.hashable[v] = rng() % 4 != 0;
            int degree = static_cast<int>(rng() % 5);
            for (int j = 0; j < degree; ++j)
                problem.edges[v].push_back({int(rng() % n), int(rng() % 13) - 6, int(rng() % 3)});
        }
        SearchConfig config;
        config.max_depth = trial % 6;
        config.deduplicate = trial % 3 != 0;
        for (int limit : {-1, 0, 1, 7, 30}) {
            config.max_expansions = limit < 0 ? nullopt : optional<uint64_t>(limit);
            for (int width : {1, 3}) {
                auto fixed = FixedBeam{size_t(width)};
                auto keyed = KeyedBeam{size_t(width)};
                auto mixed = GlobalPlusKeyedBeam{size_t(width), 1};
                check(problem, config, fixed, rule(fixed), nullopt, false);
                check(problem, config, keyed, rule(keyed), nullopt, true);
                check(problem, config, mixed, rule(mixed), nullopt, true);
            }
            check(problem, config, Chokudai(FixedBeam{1}, FixedBeam{5}), {1,0}, Rule{5,0}, false);
            check(problem, config, Chokudai(FixedBeam{2}, Unbounded{}), {2,0}, rule(Unbounded{}), false);
            check(problem, config, Chokudai(KeyedBeam{1}, KeyedBeam{2}), {0,1}, Rule{0,2}, true);
            check(problem, config, Chokudai(GlobalPlusKeyedBeam{1,1}, GlobalPlusKeyedBeam{3,2}), {1,1}, Rule{3,2}, true);
            check(problem, config, Chokudai(FixedBeam{1}, KeyedBeam{2}), {1,0}, Rule{0,2}, true);
            check(problem, config, Chokudai(KeyedBeam{1}, FixedBeam{5}), {0,1}, Rule{5,0}, true);
        }
    }
}

// No key, no default Action/Info constructors, and no CopyState undo requirement.
struct PlainInfo {
    long long cost;
    optional<uint64_t> hash;
    bool finished;
    PlainInfo(long long c, optional<uint64_t> h, bool f) : cost(c), hash(h), finished(f) {}
};
struct PlainAction { int value; explicit PlainAction(int x) : value(x) {} };
struct PlainState {
    int depth = 0;
    template<class Emit> void expand(const PlainInfo& current, Emit emit) const {
        emit(PlainState{depth + 1}, PlainAction(1), PlainInfo(current.cost - 1, depth + 1, false));
    }
};
void test_plain_and_limits() {
    CopySearch<PlainState, PlainAction, PlainInfo> search(PlainState{}, PlainInfo(0, 0, false));
    SearchConfig config;
    config.max_depth = 5;
    auto result = search.run(FixedBeam{1}, config);
    assert(result.best && result.best->cost == -5 && result.best->actions.size() == 5);
    auto repeated = search.run(Chokudai(FixedBeam{1}, Unbounded{}), config);
    assert(repeated.best && repeated.best->cost == -5);
    config.time_limit_sec = 0;
    config.max_expansions = 0;
    result = search.run(FixedBeam{1}, config);
    assert(result.stop_reason == SearchStopReason::time_limit);
    assert(!result.best && result.partial && result.partial->depth == 0);
    assert(result.stats.expanded == 0 && result.stats.generated == 0);
    config.max_depth = 0;
    result = search.run(FixedBeam{1}, config);
    assert(result.stop_reason == SearchStopReason::depth_limit && result.best && !result.partial);
    assert(result.best->actions.empty());
}

struct SlowState {
    int value = 0;
    void apply(const PlainAction&) { ++value; }
    void undo(const PlainAction&) { --value; }
    template<class Emit> void expand(const PlainInfo&, Emit emit) {
        apply(PlainAction(1));
        this_thread::sleep_for(chrono::milliseconds(5));
        emit(PlainAction(1), PlainInfo(1, nullopt, false));
        undo(PlainAction(1));
        assert(value == 0); // timeout must not abort user cleanup
    }
};
void test_emit_timeout_and_deep_history() {
    TreeSearch<SlowState, PlainAction, PlainInfo> slow(SlowState{}, PlainInfo(0, nullopt, false));
    SearchConfig config;
    config.max_depth = 2; config.time_limit_sec = 0.001;
    auto stopped = slow.run(FixedBeam{1}, config);
    assert(stopped.stop_reason == SearchStopReason::time_limit && !stopped.best);
    assert(stopped.stats.generated == 0);

    Problem problem;
    problem.edges = {{{0, -1, 0}}}; problem.finished = {false}; problem.hashable = {true};
    TreeState tree(&problem);
    TreeSearch<TreeState, Action, Info> deep(tree, problem.initial());
    config.time_limit_sec.reset(); config.max_depth = 20000;
    auto result = deep.run(FixedBeam{1}, config);
    assert(result.best && result.best->cost == -20000 && result.best->actions.size() == 20000);
}

struct TrackedAction {
    static int live, peak, copies;
    int choice;
    explicit TrackedAction(int x) : choice(x) { peak = max(peak, ++live); }
    TrackedAction(const TrackedAction& other) : TrackedAction(other.choice) { ++copies; }
    TrackedAction(TrackedAction&& other) noexcept : TrackedAction(other.choice) {}
    TrackedAction& operator=(const TrackedAction&) = default;
    ~TrackedAction() { --live; }
};
int TrackedAction::live = 0;
int TrackedAction::peak = 0;
int TrackedAction::copies = 0;
struct TrackedState {
    int depth = 0;
    void apply(const TrackedAction&) { ++depth; }
    void undo(const TrackedAction&) { --depth; }
    template<class Emit> void expand(const PlainInfo& current, Emit emit) {
        for (int i = 0; i < 100; ++i)
            emit(TrackedAction(i), PlainInfo(current.cost - i, nullopt, false));
    }
};
void test_history_reclamation() {
    assert(TrackedAction::live == 0);
    TrackedAction::peak = 0;
    {
        TreeSearch<TrackedState, TrackedAction, PlainInfo> search(TrackedState{}, PlainInfo(0, nullopt, false));
        SearchConfig config;
        config.max_depth = 100;
        auto result = search.run(FixedBeam{1}, config);
        assert(result.best && result.best->cost == -9900);
        assert(result.stats.generated == 10000);
        assert(TrackedAction::live == 199); // only the two returned paths remain
        assert(TrackedAction::peak < 1000); // rejected histories do not accumulate with Q
    }
    assert(TrackedAction::live == 0);
}

struct RejectedState : TrackedState {
    template<class Emit> void expand(const PlainInfo& current, Emit emit) {
        // The first candidate is best; the remaining 99 cannot enter width 1.
        for (int i = 99; i >= 0; --i)
            emit(TrackedAction(i), PlainInfo(current.cost - i, nullopt, false));
    }
};
void test_rejected_history_cost() {
    TrackedAction::copies = 0;
    {
        TreeSearch<RejectedState, TrackedAction, PlainInfo> search(RejectedState{}, PlainInfo(0, nullopt, false));
        SearchConfig config;
        config.max_depth = 40;
        auto result = search.run(FixedBeam{1}, config);
        assert(result.best && result.best->cost == -3960);
        assert(result.stats.generated == 4000); // rejected candidates still count
        assert(result.partial && result.partial->depth == 39 && result.partial->cost == -3861);
        assert(TrackedAction::copies < 500); // no Action copy for every rejected history
    }
    assert(TrackedAction::live == 0);
}

struct CopyProbeState {
    static int copies, moves;
    int depth = 0;
    CopyProbeState() = default;
    CopyProbeState(const CopyProbeState& other) : depth(other.depth) { ++copies; }
    CopyProbeState(CopyProbeState&& other) noexcept : depth(other.depth) { ++moves; }
    template<class Emit> void expand(const PlainInfo& current, Emit emit) const {
        for (int i = 99; i >= 0; --i) {
            auto next = *this;
            ++next.depth;
            PlainAction action(i);
            PlainInfo info(current.cost - i, nullopt, false);
            int before_copies = copies, before_moves = moves;
            if (i % 2) emit(std::move(next), action, std::move(info));
            else emit(next, action, info); // borrowing an lvalue must also work
            if (i < 99) {
                assert(copies == before_copies);
                assert(moves == before_moves);
            }
        }
    }
};
int CopyProbeState::copies = 0;
int CopyProbeState::moves = 0;
void test_rejected_state_cost() {
    CopySearch<CopyProbeState, PlainAction, PlainInfo> search(CopyProbeState{}, PlainInfo(0, nullopt, false));
    SearchConfig config;
    config.max_depth = 5;
    auto result = search.run(FixedBeam{1}, config);
    assert(result.best && result.best->cost == -495);
    assert(result.stats.generated == 500);
}

struct FirstSolutionState {
    int value = 0;
    void apply(const PlainAction&) { ++value; }
    void undo(const PlainAction&) { --value; }
    template<class Emit> void expand(const PlainInfo&, Emit emit) {
        apply(PlainAction(1));
        emit(PlainAction(1), PlainInfo(10, nullopt, true));
        undo(PlainAction(1));
        assert(value == 0); // first-solution termination must still allow cleanup
        emit(PlainAction(2), PlainInfo(-10, nullopt, true));
    }
};
void test_first_solution() {
    TreeSearch<FirstSolutionState, PlainAction, PlainInfo> search(FirstSolutionState{}, PlainInfo(0, nullopt, false));
    SearchConfig config;
    config.max_depth = 3;
    auto normal = search.run(FixedBeam{10}, config);
    assert(normal.best && normal.best->cost == -10 && normal.stats.generated == 2);
    config.stop_on_first_solution = true;
    for (bool chokudai : {false, true}) {
        auto result = chokudai ? search.run(Chokudai(FixedBeam{1}, Unbounded{}), config)
                              : search.run(FixedBeam{10}, config);
        assert(result.stop_reason == SearchStopReason::solution_found);
        assert(result.best && result.best->cost == 10 && result.best->actions[0].value == 1);
        assert(result.stats.generated == 1 && result.stats.expanded == 1);
    }
    CopySearch<PlainState, PlainAction, PlainInfo> copy(PlainState{}, PlainInfo(0, nullopt, false));
    config.max_depth = 1;
    auto result = copy.run(FixedBeam{1}, config);
    assert(result.stop_reason == SearchStopReason::solution_found && result.best && result.best->depth == 1);
    config.max_depth = 0;
    result = copy.run(FixedBeam{1}, config);
    assert(result.stop_reason == SearchStopReason::solution_found && result.best && result.best->depth == 0);
}

void test_heap_hash_replacement() {
    Problem p;
    constexpr int leaves = 700, goal = leaves + 1;
    p.edges.resize(goal+1); p.finished.assign(goal+1, false); p.hashable.assign(goal+1, true);
    p.finished[goal] = true;
    // Force wraparound probe clusters, then repeatedly improve and evict entries.
    auto mix = [](uint64_t x) {
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    };
    for (uint64_t x = 0; p.hash_values.size() < p.edges.size(); ++x)
        if ((mix(x) & 2047) == 2047) p.hash_values.push_back(x);
    mt19937 rng(287516);
    for (int i = 0; i < 2500; ++i)
        p.edges[0].push_back({1 + int(rng()%leaves), int(rng()%2001)-1000, 0});
    for (int i = 1; i <= leaves; ++i) p.edges[i].push_back({goal,int(rng()%4001)-2000,0});
    SearchConfig config; config.max_depth = 3;
    for (size_t width : {1,2,7,31,128,350}) {
        auto policy = FixedBeam{width};
        check(p, config, policy, rule(policy), nullopt, false);
    }
}

void test_dfs_order_and_clock_interval() {
    Problem p;
    p.edges.resize(127); p.finished.assign(127,false); p.hashable.assign(127,false);
    auto cost = [](int vertex) -> long long { return vertex ? ((uint32_t(vertex)*1103515245U+12345U)&65535) : 0; };
    for (int v = 0; v < 63; ++v) for (int child : {2*v+1,2*v+2})
        p.edges[v].push_back({child,int(cost(child)-cost(v)),0});
    TreeSearch<TreeState, Action, Info> search(TreeState(&p), p.initial());
    SearchConfig config; config.max_depth = 6;
    TreeState::applies = TreeState::undoes = 0;
    auto ordered = search.run(FixedBeam{64},config);
    int ordered_moves = TreeState::applies + TreeState::undoes;
    config.tree_dfs_order = true;
    TreeState::applies = TreeState::undoes = 0;
    auto dfs = search.run(FixedBeam{64},config);
    int dfs_moves = TreeState::applies + TreeState::undoes;
    equal_result(ordered,dfs); // exhaustive, unique costs: traversal does not change the answer
    assert(dfs_moves < ordered_moves);
    config.time_limit_sec = 0;
    config.time_check_interval = 256;
    auto stopped = search.run(FixedBeam{64},config);
    assert(stopped.stop_reason == SearchStopReason::time_limit && stopped.stats.expanded == 0);
}

struct MovementAudit {
    vector<int> previous{0};
    vector<pair<char,Action>> moves;
    int checked = 0;
    void check(const vector<int>& current) {
        size_t common = 0;
        while (common < min(previous.size(),current.size()) && previous[common] == current[common]) ++common;
        vector<pair<char,Action>> expected;
        for (size_t i = previous.size(); i > common; --i)
            expected.push_back({'u',{previous[i-2],previous[i-1]}});
        for (size_t i = common; i < current.size(); ++i)
            expected.push_back({'a',{current[i-1],current[i]}});
        assert(moves == expected);
        moves.clear(); previous = current; ++checked;
    }
};
struct AuditedTreeState {
    MovementAudit* audit;
    vector<int> path{0};
    void apply(const Action& action) {
        assert(path.back() == action.from);
        audit->moves.push_back({'a',action}); path.push_back(action.to);
    }
    void undo(const Action& action) {
        assert(path.back() == action.to);
        audit->moves.push_back({'u',action}); path.pop_back();
        assert(path.back() == action.from);
    }
    template<class Emit> void expand(const Info& current, Emit emit) {
        assert(current.hash == uint64_t(path.back()));
        audit->check(path);
        for (int choice = 1; choice <= 3; ++choice) {
            int next = path.back()*3+choice;
            long long cost = current.cost-int((uint32_t(next)*1103515245U+12345U)%31);
            emit(Action{path.back(),next},Info{cost,next%5,uint64_t(next),false});
        }
    }
};
void test_minimal_tree_movement() {
    auto run = [](auto policy, bool dfs) {
        MovementAudit audit;
        SearchConfig config; config.max_depth = 7; config.max_expansions = 80; config.tree_dfs_order = dfs;
        TreeSearch<AuditedTreeState,Action,Info> search(AuditedTreeState{&audit},Info{0,0,0,false});
        auto result = search.run(policy,config);
        assert(audit.checked == int(result.stats.expanded) && audit.moves.empty());
    };
    for (bool dfs : {false,true}) {
        run(FixedBeam{12},dfs);
        run(KeyedBeam{3},dfs);
        run(GlobalPlusKeyedBeam{7,2},dfs);
    }
    run(Chokudai(FixedBeam{2},FixedBeam{12}),false);
    run(Chokudai(KeyedBeam{1},GlobalPlusKeyedBeam{7,2}),false);
}

void test_changing_keys() {
    Problem p;
    constexpr int layers = 40, width = 8;
    p.edges.resize(layers*width); p.finished.assign(layers*width,false); p.hashable.assign(layers*width,true);
    for (int layer = 0; layer < layers-1; ++layer) for (int from = 0; from < width; ++from)
        for (int to = 0; to < width; ++to) {
            int child = (layer+1)*width+to;
            p.edges[layer*width+from].push_back({child,(from*17+to*13+layer)%11-5,-child/2});
            p.hashable[child] = to%3 != 0;
        }
    for (int i = (layers-1)*width; i < layers*width; ++i) p.finished[i] = true;
    SearchConfig config; config.max_depth = layers;
    for (size_t count : {1,2,7}) {
        auto policy = KeyedBeam{count};
        check(p,config,policy,rule(policy),nullopt,true);
    }
}

struct alignas(64) WideAction {
    static int live, copies_until_throw;
    int value;
    array<uint64_t,16> data;
    explicit WideAction(int x) : value(x) { data.fill(uint64_t(x)); ++live; }
    WideAction(const WideAction& other) : value(other.value), data(other.data) {
        if (copies_until_throw == 0) throw runtime_error("action copy failure");
        if (copies_until_throw > 0) --copies_until_throw;
        ++live;
    }
    ~WideAction() { --live; }
};
int WideAction::live = 0;
int WideAction::copies_until_throw = -1;
struct WideActionState {
    int value = 0, depth = 0;
    void apply(const WideAction& action) {
        assert(all_of(action.data.begin(),action.data.end(),[&](uint64_t x) { return x == uint64_t(action.value); }));
        value += action.value; ++depth;
    }
    void undo(const WideAction& action) { value -= action.value; --depth; }
    template<class Emit> void expand(const PlainInfo&, Emit emit) {
        for (int step : {1,2})
            emit(WideAction(step),PlainInfo(-value-step,(uint64_t(depth+1)<<32)|uint64_t(value+step),false));
    }
};
void test_large_action_lifetime() {
    TreeSearch<WideActionState,WideAction,PlainInfo> search(WideActionState{},PlainInfo(0,0,false));
    SearchConfig config; config.max_depth = 60;
    {
        auto result = search.run(FixedBeam{32},config);
        assert(result.best && result.best->cost == -120 && result.best->actions.size() == 60);
        for (const auto& action : result.best->actions) assert(action.value == 2);
    }
    assert(WideAction::live == 0);
    WideAction::copies_until_throw = 30;
    bool threw = false;
    try { search.run(FixedBeam{32},config); }
    catch (const runtime_error&) { threw = true; }
    assert(threw && WideAction::live == 0);
    WideAction::copies_until_throw = -1;
    {
        auto result = search.run(Chokudai(FixedBeam{2},FixedBeam{32}),config);
        assert(result.best && result.best->cost == -120);
    }
    assert(WideAction::live == 0);
}

struct alignas(64) WideKey {
    array<uint64_t,96> words;
    explicit WideKey(int value) { words.fill(uint64_t(value)); }
    bool operator==(const WideKey& other) const { return words == other.words; }
};
namespace std {
template<> struct hash<WideKey> {
    size_t operator()(const WideKey& key) const noexcept { return hash<uint64_t>{}(key.words[0]); }
};
}
struct AlignedKeyInfo {
    using Key = WideKey;
    long long cost;
    optional<uint64_t> hash;
    bool finished;
    Key key;
    AlignedKeyInfo(long long c, optional<uint64_t> h, bool f)
        : cost(c), hash(h ? h : optional<uint64_t>(uint64_t(-c))), finished(f), key(int(c%17)) {}
};
struct alignas(256) AlignedState {
    static int live, copies_until_throw;
    int depth, value;
    array<int,256> data;
    AlignedState(int d = 0, int v = 0) : depth(d), value(v) { data.fill(v); ++live; }
    AlignedState(const AlignedState& other) : depth(other.depth), value(other.value), data(other.data) {
        if (copies_until_throw == 0) throw runtime_error("copy failure");
        if (copies_until_throw > 0) --copies_until_throw;
        ++live;
    }
    ~AlignedState() { --live; }
    template<class Information, class Emit> void expand(const Information& current, Emit emit) const {
        assert(reinterpret_cast<uintptr_t>(this) % alignof(AlignedState) == 0);
        assert(all_of(data.begin(), data.end(), [&](int x) { return x == value; }));
        int count = depth == 0 ? 700 : 1;
        for (int i = 0; i < count; ++i) {
            AlignedState child(depth+1, value+i);
            emit(child, PlainAction(i), Information(current.cost-(depth == 0 ? i : 1),nullopt,false));
        }
    }
};
int AlignedState::live = 0;
int AlignedState::copies_until_throw = -1;
void test_aligned_state_and_exception_cleanup() {
    {
        CopySearch<AlignedState,PlainAction,PlainInfo> search(AlignedState{},PlainInfo(0,nullopt,false));
        SearchConfig config; config.max_depth = 3;
        auto check_cleanup = [&](auto& instance, auto policy) {
            int saved_live = AlignedState::live;
            auto result = instance.run(policy,config);
            assert(result.best && result.best->cost == -701);
            assert(AlignedState::live == saved_live);
            AlignedState::copies_until_throw = 50;
            bool threw = false;
            try { instance.run(policy,config); }
            catch (const runtime_error&) { threw = true; }
            assert(threw && AlignedState::live == saved_live);
            AlignedState::copies_until_throw = -1;
            auto again = instance.run(policy,config);
            assert(again.best && again.best->cost == -701);
        };
        check_cleanup(search,FixedBeam{350});
        check_cleanup(search,Chokudai(FixedBeam{1},FixedBeam{350}));
        CopySearch<AlignedState,PlainAction,AlignedKeyInfo> keyed(AlignedState{},AlignedKeyInfo(0,nullopt,false));
        check_cleanup(keyed,KeyedBeam{30});
        check_cleanup(keyed,GlobalPlusKeyedBeam{100,10});
        check_cleanup(keyed,Chokudai(KeyedBeam{1},GlobalPlusKeyedBeam{100,10}));
    }
    assert(AlignedState::live == 0);
}

int main() {
    test_oracle();
    test_plain_and_limits();
    test_emit_timeout_and_deep_history();
    test_history_reclamation();
    test_rejected_history_cost();
    test_rejected_state_cost();
    test_first_solution();
    test_heap_hash_replacement();
    test_dfs_order_and_clock_interval();
    test_minimal_tree_movement();
    test_changing_keys();
    test_large_action_lifetime();
    test_aligned_state_and_exception_cleanup();
    cout << "Hello World\n";
}
