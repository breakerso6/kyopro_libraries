// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/main/libraries/algorithm/BeamSearch.hpp
// SPDX-License-Identifier: CC0-1.0
#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <new>
#include <optional>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

struct FixedBeam { std::size_t width; };
struct KeyedBeam { std::size_t per_key_width; };
struct GlobalPlusKeyedBeam { std::size_t global_width, per_key_width; };
struct Unbounded {};

template<class Expand, class Frontier>
struct Chokudai {
    Expand expand_per_depth;
    Frontier frontier_limit;
    Chokudai(Expand expand, Frontier frontier)
        : expand_per_depth(std::move(expand)), frontier_limit(std::move(frontier)) {}
};

struct SearchConfig {
    int max_depth = 0;
    std::optional<double> time_limit_sec;
    std::optional<std::uint64_t> max_expansions;
    bool deduplicate = true;
    bool stop_on_first_solution = false;
    std::uint32_t time_check_interval = 1; // 1 checks every boundary; larger values amortize clock reads.
    bool tree_dfs_order = false; // Fixed-depth TreeSearch only; changes expansion/tie/random order.
};

enum class SearchStopReason { depth_limit, frontier_empty, time_limit, expansion_limit, solution_found };

struct SearchStats {
    std::uint64_t expanded = 0;
    std::uint64_t generated = 0;
    std::uint64_t duplicates = 0;
    double elapsed_sec = 0;
};

template<class Action, class Cost>
struct SearchPath {
    std::vector<Action> actions;
    Cost cost;
    int depth;
};

template<class Action, class Cost>
struct SearchResult {
    std::optional<SearchPath<Action, Cost>> best;
    std::optional<SearchPath<Action, Cost>> partial;
    SearchStats stats;
    SearchStopReason stop_reason = SearchStopReason::frontier_empty;
};

namespace beam_search_detail {

template<class P> constexpr bool uses_key =
    std::is_same_v<P, KeyedBeam> || std::is_same_v<P, GlobalPlusKeyedBeam>;
template<class P> constexpr bool is_selection =
    uses_key<P> || std::is_same_v<P, FixedBeam>;

inline void validate(FixedBeam p) { assert(p.width > 0); (void)p; }
inline void validate(KeyedBeam p) { assert(p.per_key_width > 0); (void)p; }
inline void validate(GlobalPlusKeyedBeam p) {
    assert(p.global_width > 0 && p.per_key_width > 0); (void)p;
}
inline void validate(Unbounded) {}

// Key-free instantiations never name Info::Key or access Info::key.
template<class Info, bool Keyed> struct Identity;
template<class Info> struct Identity<Info, false> {
    using Key = unsigned char;
    using Id = std::uint64_t;
    using Hash = std::hash<Id>;
    static Id id(const Info& info) { return *info.hash; }
};
template<class Info> struct Identity<Info, true> {
    using Key = typename Info::Key;
    struct Id {
        Key key;
        std::uint64_t hash;
        bool operator==(const Id& other) const {
            return hash == other.hash && key == other.key;
        }
    };
    struct Hash {
        std::size_t operator()(const Id& value) const {
            auto a = std::hash<Key>{}(value.key);
            auto b = std::hash<std::uint64_t>{}(value.hash);
            return a ^ (b + std::size_t(0x9e3779b9U) + (a << 6) + (a >> 2));
        }
    };
    static Id id(const Info& info) { return {info.key, *info.hash}; }
};

// Stable addresses, without default-constructing State or Action in unused slots.
template<class Node>
class NodePool {
    union Slot {
        Node node;
        Slot* next;
        Slot() : next(nullptr) {}
        ~Slot() {}
    };
    static constexpr std::size_t count = std::max(std::size_t(1),
        std::min(std::size_t(256), std::size_t(32768) / sizeof(Slot)));
    std::vector<Slot*> blocks_;
    Slot* free_ = nullptr;
public:
    NodePool() = default;
    NodePool(const NodePool&) = delete;
    NodePool& operator=(const NodePool&) = delete;
    ~NodePool() {
        for (Slot* block : blocks_) {
            for (std::size_t i = 0; i < count; ++i) block[i].~Slot();
            std::allocator<Slot>{}.deallocate(block,count);
        }
    }
    void* allocate() {
        if (!free_) {
            Slot* block = std::allocator<Slot>{}.allocate(count);
            try { blocks_.push_back(block); }
            catch (...) { std::allocator<Slot>{}.deallocate(block,count); throw; }
            for (std::size_t i = 0; i < count; ++i) new (block+i) Slot;
            for (std::size_t i = 0; i + 1 < count; ++i) block[i].next = &block[i+1];
            free_ = block;
        }
        Slot* slot = free_;
        free_ = slot->next;
        return static_cast<void*>(&slot->node);
    }
    void deallocate(Node* node) noexcept {
        auto* slot = reinterpret_cast<Slot*>(node);
        slot->next = free_;
        free_ = slot;
    }
};

// Intrusive trace references use an iterative release, even for very deep paths.
// Only live candidates, the cursor, and their ancestors retain trace records.
template<class Action>
class TraceArena {
    struct Record {
        Record* parent;
        std::size_t refs;
        int depth;
        std::optional<Action> action;
        Record(Record* p, int d, const Action* a) : parent(p), refs(1), depth(d) {
            if (a) action.emplace(*a);
        }
    };
    NodePool<Record> pool_;
    void retain(Record* record) { ++record->refs; }
    void release(Record* record) {
        while (record) {
            if (--record->refs != 0) return;
            Record* parent = record->parent;
            record->~Record();
            pool_.deallocate(record);
            record = parent;
        }
    }
public:
    using Id = Record*;
    class Handle {
        TraceArena* arena_ = nullptr;
        Id id_ = nullptr;
        friend class TraceArena;
        Handle(TraceArena* arena, Id id) : arena_(arena), id_(id) {}
    public:
        Handle() = default;
        Handle(const Handle& other) : arena_(other.arena_), id_(other.id_) {
            if (arena_) arena_->retain(id_);
        }
        Handle(Handle&& other) noexcept
            : arena_(std::exchange(other.arena_,nullptr)), id_(other.id_) {}
        Handle& operator=(Handle other) noexcept {
            std::swap(arena_,other.arena_);
            std::swap(id_,other.id_);
            return *this;
        }
        ~Handle() { if (arena_) arena_->release(id_); }
        Id id() const { return id_; }
    };
    Handle root() { return allocate(nullptr,0,nullptr); }
    Handle child(const Handle& parent, const Action& action) {
        auto result = allocate(parent.id_,parent.id_->depth+1,&action);
        retain(parent.id_);
        return result;
    }
    int depth(Id id) const { return id->depth; }
    Id parent(Id id) const { return id->parent; }
    const Action& action(Id id) const { return *id->action; }
    std::vector<Action> path(const Handle& handle) const {
        std::vector<Id> ids;
        for (auto id = handle.id_; id->parent; id = id->parent) ids.push_back(id);
        std::vector<Action> result;
        result.reserve(ids.size());
        for (auto it = ids.rbegin(); it != ids.rend(); ++it) result.push_back(action(*it));
        return result;
    }
private:
    Handle allocate(Id parent, int depth, const Action* action) {
        void* memory = pool_.allocate();
        try {
            return Handle(this,new (memory) Record(parent,depth,action));
        } catch (...) {
            pool_.deallocate(static_cast<Record*>(memory));
            throw;
        }
    }
};

template<class State, class Action, class Info, bool Tree>
struct Candidate {
    using Information = Info;
    using Payload = std::conditional_t<Tree, std::monostate, State>;
    Info info;
    typename TraceArena<Action>::Handle trace;
    Payload state;
    std::uint64_t serial;
    bool global = false;
    template<class I, class P>
    Candidate(I&& value, typename TraceArena<Action>::Handle history,
              P&& payload, std::uint64_t order)
        : info(std::forward<I>(value)), trace(std::move(history)),
          state(std::forward<P>(payload)), serial(order) {}
};

template<class Node>
struct PoolDelete {
    NodePool<Node>* resource = nullptr;
    void operator()(Node* node) const noexcept {
        node->~Node();
        resource->deallocate(node);
    }
};
template<class Node> using UniqueNode = std::unique_ptr<Node, PoolDelete<Node>>;

// Small index nodes share storage across all frontiers in one search. Bucket
// arrays and unusually large/aligned user keys keep the standard allocator.
class IndexPool {
    struct Link { Link* next; };
    struct Bucket {
        Link* free = nullptr;
        std::vector<void*> blocks;
        ~Bucket() { for (void* block : blocks) ::operator delete(block); }
    };
    static constexpr std::size_t unit = alignof(void*), classes = 64;
    std::unique_ptr<Bucket[]> buckets_;
    template<class T> static constexpr std::size_t alignment = std::max(alignof(T), unit);
    template<class T> static constexpr std::size_t stride =
        (sizeof(T) + alignment<T> - 1) / alignment<T> * alignment<T>;
public:
    template<class T> static constexpr bool pooled =
        alignof(T) <= alignof(std::max_align_t) && stride<T> <= unit*classes;
    template<class T> T* allocate() {
        static_assert(pooled<T>);
        constexpr std::size_t size = stride<T>, count = std::max(std::size_t(1),
            std::min(std::size_t(256), std::size_t(32768)/size));
        if (!buckets_) buckets_ = std::make_unique<Bucket[]>(classes);
        auto& bucket = buckets_[size/unit-1];
        if (!bucket.free) {
            void* block = ::operator new(size*count);
            try { bucket.blocks.push_back(block); }
            catch (...) { ::operator delete(block); throw; }
            Link* next = nullptr;
            for (std::size_t i = count; i > 0; --i)
                next = new (static_cast<char*>(block)+(i-1)*size) Link{next};
            bucket.free = next;
        }
        Link* link = bucket.free;
        bucket.free = link->next;
        return reinterpret_cast<T*>(link);
    }
    template<class T> void deallocate(T* pointer) noexcept {
        auto& bucket = buckets_[stride<T>/unit-1];
        bucket.free = new (static_cast<void*>(pointer)) Link{bucket.free};
    }
};

template<class T> struct IndexAllocator {
    using value_type = T;
    IndexPool* pool = nullptr;
    IndexAllocator() = default;
    explicit IndexAllocator(IndexPool& resource) : pool(&resource) {}
    template<class U> IndexAllocator(const IndexAllocator<U>& other) noexcept : pool(other.pool) {}
    T* allocate(std::size_t n) {
        if constexpr (IndexPool::pooled<T>) {
            if (pool && n == 1) return pool->allocate<T>();
        }
        return std::allocator<T>{}.allocate(n);
    }
    void deallocate(T* pointer, std::size_t n) noexcept {
        if constexpr (IndexPool::pooled<T>) {
            if (pool && n == 1) { pool->deallocate(pointer); return; }
        }
        std::allocator<T>{}.deallocate(pointer,n);
    }
    template<class U> bool operator==(const IndexAllocator<U>& other) const noexcept { return pool == other.pool; }
    template<class U> bool operator!=(const IndexAllocator<U>& other) const noexcept { return !(*this == other); }
};
template<class K, class V, class Hash = std::hash<K>>
using IndexMap = std::unordered_map<K,V,Hash,std::equal_to<K>,IndexAllocator<std::pair<const K,V>>>;

// All retained nodes have a cost-ordered index. Mixed selection additionally
// partitions them into global winners and per-key extras, updated in O(log N).
template<class Node, class Policy, bool Keyed>
class Frontier {
    using Info = typename Node::Information;
    using Traits = Identity<Info, Keyed>;
    using Key = typename Traits::Key;
public:
    using Ptr = UniqueNode<Node>;
private:
    struct Less {
        bool operator()(const Node* a, const Node* b) const {
            if (a->info.cost < b->info.cost) return true;
            if (b->info.cost < a->info.cost) return false;
            return a->serial < b->serial;
        }
    };
    using Set = std::set<Node*, Less, IndexAllocator<Node*>>;
    Policy policy_;
    bool deduplicate_;
    NodePool<Node>* pool_;
    IndexPool* indices_;
    Set ordered_, globals_;
    std::vector<Node*> selected_;
    IndexMap<Key, std::size_t> counts_;
    IndexMap<Key, Set> extras_;
    IndexMap<typename Traits::Id, Node*, typename Traits::Hash> hashes_;

    void remove_extra(Node* node) {
        if constexpr (uses_key<Policy>) {
            auto it = extras_.find(node->info.key);
            it->second.erase(node);
            if (it->second.empty()) extras_.erase(it);
        }
    }
    void add_extra(Node* node) {
        if constexpr (uses_key<Policy>) {
            auto& bucket = extras_.try_emplace(node->info.key, Less{},
                IndexAllocator<Node*>{*indices_}).first->second;
            bucket.insert(node);
            if (bucket.size() > policy_.per_key_width) {
                Node* worst = *bucket.rbegin();
                erase(worst);
            }
        }
    }
public:
    Frontier(Policy policy, bool deduplicate, NodePool<Node>& pool, IndexPool& indices)
        : policy_(policy), deduplicate_(deduplicate), pool_(&pool), indices_(&indices),
          ordered_(Less{},IndexAllocator<Node*>{indices}), globals_(Less{},IndexAllocator<Node*>{indices}),
          counts_(IndexAllocator<typename decltype(counts_)::value_type>{indices}),
          extras_(IndexAllocator<typename decltype(extras_)::value_type>{indices}),
          hashes_(IndexAllocator<typename decltype(hashes_)::value_type>{indices}) { validate(policy); }
    Frontier(const Frontier&) = delete;
    Frontier& operator=(const Frontier&) = delete;
    Frontier(Frontier&&) = default;
    Frontier& operator=(Frontier&& other) {
        clear();
        policy_ = std::move(other.policy_); deduplicate_ = other.deduplicate_; pool_ = other.pool_; indices_ = other.indices_;
        ordered_ = std::move(other.ordered_); globals_ = std::move(other.globals_);
        extras_ = std::move(other.extras_); hashes_ = std::move(other.hashes_);
        selected_ = std::move(other.selected_);
        counts_ = std::move(other.counts_);
        return *this;
    }
    ~Frontier() { clear(); }
    void clear() {
        for (Node* node : ordered_) PoolDelete<Node>{pool_}(node);
        ordered_.clear(); globals_.clear(); extras_.clear(); hashes_.clear(); counts_.clear();
    }
    bool empty() const { return ordered_.empty(); }

    // Called before allocating a candidate/history. All incoming candidates are
    // newer than retained candidates, so equal-cost ties can be rejected here.
    bool prepare(const Info& info, SearchStats& stats) {
        if (deduplicate_ && info.hash) {
            auto it = hashes_.find(Traits::id(info));
            if (it != hashes_.end()) {
                ++stats.duplicates;
                Node* old = it->second;
                if (!(info.cost < old->info.cost)) return false;
                erase(old);
            }
        }
        if constexpr (std::is_same_v<Policy, FixedBeam>) {
            return ordered_.size() < policy_.width || info.cost < (*ordered_.rbegin())->info.cost;
        } else if constexpr (uses_key<Policy>) {
            if constexpr (std::is_same_v<Policy, GlobalPlusKeyedBeam>) {
                if (globals_.size() < policy_.global_width || info.cost < (*globals_.rbegin())->info.cost)
                    return true;
            }
            auto it = extras_.find(info.key);
            return it == extras_.end() || it->second.size() < policy_.per_key_width ||
                   info.cost < (*it->second.rbegin())->info.cost;
        } else {
            return true;
        }
    }

    void insert(Ptr node, SearchStats& stats) {
        if (prepare(node->info, stats)) insert_prepared(std::move(node));
    }

    void insert_prepared(Ptr owned) {
        Node* node = owned.get();
        ordered_.insert(node);
        owned.release(); // ordered_ owns each node; other indices only borrow it.
        if (deduplicate_ && node->info.hash) hashes_.emplace(Traits::id(node->info), node);
        if constexpr (std::is_same_v<Policy, FixedBeam>) {
            if (ordered_.size() > policy_.width) {
                Node* worst = *ordered_.rbegin();
                erase(worst);
            }
        } else if constexpr (std::is_same_v<Policy, KeyedBeam>) {
            add_extra(node);
        } else if constexpr (std::is_same_v<Policy, GlobalPlusKeyedBeam>) {
            if (globals_.size() < policy_.global_width || Less{}(node, *globals_.rbegin())) {
                node->global = true;
                globals_.insert(node);
                if (globals_.size() > policy_.global_width) {
                    Node* displaced = *globals_.rbegin();
                    globals_.erase(displaced);
                    displaced->global = false;
                    add_extra(displaced);
                }
            } else {
                add_extra(node);
            }
        } else {
            static_assert(std::is_same_v<Policy, Unbounded>, "Unsupported frontier policy");
        }
    }

    Ptr detach(Node* node) {
        Ptr owned(node, PoolDelete<Node>{pool_});
        ordered_.erase(node);
        if constexpr (std::is_same_v<Policy, GlobalPlusKeyedBeam>) {
            if (node->global) globals_.erase(node);
            else remove_extra(node);
        } else if constexpr (std::is_same_v<Policy, KeyedBeam>) {
            remove_extra(node);
        }
        if (deduplicate_ && node->info.hash) hashes_.erase(Traits::id(node->info));
        if constexpr (std::is_same_v<Policy, GlobalPlusKeyedBeam>) {
            if (globals_.size() < policy_.global_width && ordered_.size() > globals_.size()) {
                auto it = globals_.empty() ? ordered_.begin() : ordered_.upper_bound(*globals_.rbegin());
                Node* promoted = *it;
                remove_extra(promoted);
                promoted->global = true;
                globals_.insert(promoted);
            }
        }
        return owned;
    }

    void erase(Node* node) { auto owned = detach(node); }

    // A finished generation will never receive another insertion. Clear its
    // indexes directly instead of rebalancing global/key quotas for every pop.
    void drain(std::vector<Ptr>& result, bool by_serial = false) {
        result.clear();
        result.reserve(ordered_.size());
        for (Node* node : ordered_) result.emplace_back(node, PoolDelete<Node>{pool_});
        ordered_.clear(); globals_.clear(); extras_.clear(); hashes_.clear();
        if (by_serial) std::sort(result.begin(), result.end(), [](const Ptr& a, const Ptr& b) {
            return a->serial < b->serial;
        });
    }

    template<class Selection>
    void take(Selection selection, std::vector<Ptr>& result) {
        validate(selection);
        result.clear();
        selected_.clear();
        if constexpr (std::is_same_v<Selection, FixedBeam>) {
            selected_.reserve(std::min(selection.width, ordered_.size()));
            auto it = ordered_.begin();
            while (it != ordered_.end() && selected_.size() < selection.width)
                selected_.push_back(*it++);
        } else {
            static_assert(uses_key<Selection>, "Unsupported expansion policy");
            [[maybe_unused]] std::size_t global_count = 0;
            if (counts_.size() > 2*ordered_.size()+64) counts_.clear();
            else for (auto& entry : counts_) entry.second = 0;
            for (const auto& node : ordered_) {
                if constexpr (std::is_same_v<Selection, GlobalPlusKeyedBeam>) {
                    if (global_count < selection.global_width) {
                        ++global_count;
                        selected_.push_back(node);
                        continue;
                    }
                }
                auto& count = counts_[node->info.key];
                if (count < selection.per_key_width) {
                    ++count;
                    selected_.push_back(node);
                }
            }
        }
        result.reserve(selected_.size());
        for (Node* node : selected_) result.push_back(detach(node));
    }
};

// Fixed-width generations need only the worst retained candidate during
// insertion. A contiguous indexed heap avoids ordered-tree allocations.
template<class Node>
class FixedSelector {
    using Info = typename Node::Information;
    using Cost = std::decay_t<decltype(std::declval<Info>().cost)>;
    static constexpr std::size_t none = std::numeric_limits<std::size_t>::max();
public:
    using Ptr = UniqueNode<Node>;
private:
    struct Entry {
        Ptr node;
        Cost cost;
        std::uint64_t serial;
        std::size_t bucket = none;
    };
    struct Bucket {
        std::uint64_t hash = 0;
        std::size_t index = none;
    };
    std::size_t width_;
    bool deduplicate_;
    std::vector<Entry> heap_;
    std::vector<std::size_t> order_;
    std::vector<Bucket> table_;
    std::size_t hashes_ = 0;
    std::size_t replace_ = none;
    bool keep_bucket_ = false;

    static bool better(const Entry& a, const Entry& b) {
        if (a.cost < b.cost) return true;
        if (b.cost < a.cost) return false;
        return a.serial < b.serial;
    }
    static std::uint64_t mix(std::uint64_t x) {
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }
    std::size_t home(std::uint64_t hash) const { return mix(hash) & (table_.size() - 1); }
    std::size_t find(std::uint64_t hash) const {
        if (table_.empty()) return none;
        std::size_t slot = home(hash), mask = table_.size() - 1;
        while (table_[slot].index != none && table_[slot].hash != hash) slot = (slot + 1) & mask;
        return slot;
    }
    void update(std::size_t index) {
        if (heap_[index].bucket != none) table_[heap_[index].bucket].index = index;
    }
    void up(std::size_t index) {
        Entry value = std::move(heap_[index]);
        while (index > 0) {
            auto parent = (index - 1) / 2;
            if (!better(heap_[parent], value)) break;
            heap_[index] = std::move(heap_[parent]);
            update(index); index = parent;
        }
        heap_[index] = std::move(value); update(index);
    }
    void down(std::size_t index) {
        Entry value = std::move(heap_[index]);
        for (;;) {
            auto child = index * 2 + 1;
            if (child >= heap_.size()) break;
            if (child + 1 < heap_.size() && better(heap_[child], heap_[child+1])) ++child;
            if (!better(value, heap_[child])) break;
            heap_[index] = std::move(heap_[child]);
            update(index); index = child;
        }
        heap_[index] = std::move(value); update(index);
    }
    void erase_bucket(std::size_t hole) {
        if (hole == none) return;
        std::size_t mask = table_.size() - 1;
        table_[hole].index = none;
        --hashes_;
        for (std::size_t next = (hole + 1) & mask; table_[next].index != none; next = (next + 1) & mask) {
            auto start = home(table_[next].hash);
            if (((next - start) & mask) >= ((next - hole) & mask)) {
                table_[hole] = table_[next];
                heap_[table_[hole].index].bucket = hole;
                table_[next].index = none;
                hole = next;
            }
        }
    }
    void rebuild(std::size_t capacity) {
        table_.assign(capacity, Bucket{});
        hashes_ = 0;
        for (std::size_t i = 0; i < heap_.size(); ++i) {
            heap_[i].bucket = none;
            if (heap_[i].node->info.hash) {
                auto hash = *heap_[i].node->info.hash;
                auto slot = find(hash);
                table_[slot] = Bucket{hash, i};
                heap_[i].bucket = slot;
                ++hashes_;
            }
        }
    }
    void bind(std::size_t index) {
        if (!deduplicate_ || !heap_[index].node->info.hash) return;
        if (table_.empty()) {
            // Tiny per-key queues can compare hashes directly without a separate table.
            if (heap_.size() <= 4) return;
            rebuild(32);
            return;
        }
        if (hashes_ + 1 > table_.size() / 4) {
            rebuild(table_.size() * 2);
            return;
        }
        auto hash = *heap_[index].node->info.hash;
        auto slot = find(hash);
        table_[slot] = Bucket{hash, index};
        heap_[index].bucket = slot;
        ++hashes_;
    }
public:
    FixedSelector(FixedBeam policy, bool deduplicate, NodePool<Node>&, IndexPool&,
                  std::size_t initial_capacity = 1024)
        : width_(policy.width), deduplicate_(deduplicate) {
        validate(policy);
        heap_.reserve(std::min(width_, initial_capacity));
    }
    bool empty() const { return heap_.empty(); }
    std::size_t size() const { return heap_.size(); }
    bool prepare(const Info& info, SearchStats& stats) {
        replace_ = none;
        if (deduplicate_ && info.hash) {
            if (table_.empty() && heap_.size() > 4) rebuild(32);
            std::size_t index = none;
            if (table_.empty()) {
                for (std::size_t i = 0; i < heap_.size(); ++i)
                    if (heap_[i].node->info.hash == info.hash) { index = i; break; }
            } else {
                auto slot = find(*info.hash);
                if (table_[slot].index != none) index = table_[slot].index;
            }
            if (index != none) {
                ++stats.duplicates;
                if (!(info.cost < heap_[index].cost)) return false;
                replace_ = index;
                keep_bucket_ = true;
                heap_[index].node.reset();
                return true;
            }
        }
        if (heap_.size() < width_) return true;
        if (!(info.cost < heap_[0].cost)) return false;
        replace_ = 0;
        keep_bucket_ = false;
        erase_bucket(heap_[0].bucket);
        heap_[0].bucket = none;
        heap_[0].node.reset();
        return true;
    }
    void insert(Ptr node, SearchStats& stats) {
        if (prepare(node->info, stats)) insert_prepared(std::move(node));
    }
    void insert_prepared(Ptr node) {
        Cost cost = node->info.cost;
        auto serial = node->serial;
        if (replace_ != none) {
            auto index = std::exchange(replace_,none);
            auto& entry = heap_[index];
            entry.node = std::move(node);
            entry.cost = cost;
            entry.serial = serial;
            if (!keep_bucket_) bind(index);
            down(index); // Improving a max-heap entry only moves it down.
            return;
        }
        heap_.push_back(Entry{std::move(node), cost, serial, none});
        bind(heap_.size()-1);
        up(heap_.size()-1);
    }
    void drain(std::vector<Ptr>& result, bool by_serial = false) {
        order_.resize(heap_.size());
        for (std::size_t i = 0; i < order_.size(); ++i) order_[i] = i;
        if (by_serial) std::sort(order_.begin(), order_.end(), [&](std::size_t a, std::size_t b) {
            return heap_[a].serial < heap_[b].serial;
        });
        else std::sort(order_.begin(), order_.end(), [&](std::size_t a, std::size_t b) {
            return better(heap_[a],heap_[b]);
        });
        result.clear();
        result.reserve(heap_.size());
        for (auto index : order_) result.push_back(std::move(heap_[index].node));
        heap_.clear();
        for (auto& bucket : table_) bucket.index = none;
        hashes_ = 0;
    }
    void append_unordered(std::vector<Ptr>& result) {
        for (auto& entry : heap_) result.push_back(std::move(entry.node));
        heap_.clear();
        for (auto& bucket : table_) bucket.index = none;
        hashes_ = 0;
    }
};

// Generation-based keyed selection needs only a bounded heap per key. Merge
// the survivors once at the generation boundary to retain global tie order.
template<class Node>
class KeyedSelector {
    using Info = typename Node::Information;
    using Key = typename Info::Key;
    using Heap = FixedSelector<Node>;
    using Map = IndexMap<Key,Heap>;
    KeyedBeam policy_;
    bool deduplicate_;
    NodePool<Node>* pool_;
    IndexPool* indices_;
    Map heaps_;
    Heap* pending_ = nullptr;
    std::size_t size_ = 0;
public:
    using Ptr = UniqueNode<Node>;
    KeyedSelector(KeyedBeam policy, bool deduplicate, NodePool<Node>& pool, IndexPool& indices)
        : policy_(policy), deduplicate_(deduplicate), pool_(&pool), indices_(&indices),
          heaps_(IndexAllocator<typename Map::value_type>{indices}) { validate(policy); }
    bool empty() const { return size_ == 0; }
    bool prepare(const Info& info, SearchStats& stats) {
        pending_ = &heaps_.try_emplace(info.key,FixedBeam{policy_.per_key_width},
            deduplicate_,*pool_,*indices_,std::size_t(1)).first->second;
        return pending_->prepare(info,stats);
    }
    void insert(Ptr node, SearchStats& stats) {
        if (prepare(node->info,stats)) insert_prepared(std::move(node));
    }
    void insert_prepared(Ptr node) {
        auto before = pending_->size();
        pending_->insert_prepared(std::move(node));
        size_ += pending_->size()-before;
    }
    void drain(std::vector<Ptr>& result, bool by_serial = false) {
        result.clear();
        result.reserve(size_);
        for (auto it = heaps_.begin(); it != heaps_.end();) {
            // Reuse recently active keys, but do not accumulate every key ever seen.
            if (it->second.empty()) it = heaps_.erase(it);
            else { it->second.append_unordered(result); ++it; }
        }
        size_ = 0;
        pending_ = nullptr;
        if (by_serial) std::sort(result.begin(),result.end(),[](const Ptr& a,const Ptr& b) {
            return a->serial < b->serial;
        });
        else std::sort(result.begin(),result.end(),[](const Ptr& a,const Ptr& b) {
            if (a->info.cost < b->info.cost) return true;
            if (b->info.cost < a->info.cost) return false;
            return a->serial < b->serial;
        });
    }
};

template<class State, class Action, class Info, bool Tree, bool Keyed>
class Run {
    using Cost = std::decay_t<decltype(std::declval<Info>().cost)>;
    using Arena = TraceArena<Action>;
    using Handle = typename Arena::Handle;
    using Node = Candidate<State, Action, Info, Tree>;
    using Ptr = UniqueNode<Node>;
    using Result = SearchResult<Action, Cost>;
    using Clock = std::chrono::steady_clock;

    const State& initial_;
    const Info& initial_info_;
    SearchConfig config_;
    Clock::time_point start_ = Clock::now();
    Result result_;
    NodePool<Node> candidate_pool_;
    IndexPool index_pool_;
    Arena arena_;
    Handle root_, cursor_;
    struct Saved { Cost cost; int depth; Handle trace; };
    std::optional<Saved> best_, partial_;
    std::optional<State> work_;
    std::vector<typename Arena::Id> down_;
    std::uint64_t serial_ = 0;
    bool stopped_ = false;
    std::uint32_t clock_countdown_ = 1;

    double elapsed() const { return std::chrono::duration<double>(Clock::now() - start_).count(); }
    bool timed_out() {
        if (stopped_) return true;
        if (config_.time_limit_sec && --clock_countdown_ == 0) {
            clock_countdown_ = config_.time_check_interval;
            if (elapsed() >= *config_.time_limit_sec) {
                stopped_ = true;
                result_.stop_reason = SearchStopReason::time_limit;
            }
        }
        return stopped_;
    }
    bool budget() {
        if (timed_out()) return false;
        if (config_.max_expansions && result_.stats.expanded >= *config_.max_expansions) {
            stopped_ = true;
            result_.stop_reason = SearchStopReason::expansion_limit;
        }
        return !stopped_;
    }
    bool improves(const Info& info, int depth, bool terminal) const {
        if constexpr (std::is_floating_point_v<Cost>) assert(std::isfinite(info.cost));
        const auto& slot = terminal ? best_ : partial_;
        return !slot || (!terminal && depth > slot->depth) ||
               ((terminal || depth == slot->depth) && info.cost < slot->cost);
    }
    bool consider(const Info& info, const Handle& trace) {
        int depth = arena_.depth(trace.id());
        bool terminal = info.finished || depth == config_.max_depth;
        if (improves(info, depth, terminal)) {
            auto& slot = terminal ? best_ : partial_;
            slot = Saved{info.cost, depth, trace};
        }
        return terminal;
    }
    template<class... Args>
    Ptr make_node(Args&&... args) {
        void* memory = candidate_pool_.allocate();
        try {
            Node* node = new (memory) Node(std::forward<Args>(args)...);
            return Ptr(node, PoolDelete<Node>{&candidate_pool_});
        } catch (...) {
            candidate_pool_.deallocate(static_cast<Node*>(memory));
            throw;
        }
    }
    Ptr root_node() {
        if constexpr (Tree)
            return make_node(initial_info_, root_, std::monostate{}, serial_++);
        else
            return make_node(initial_info_, root_, initial_, serial_++);
    }
    void move_to(Handle& target) {
        auto a = cursor_.id(), b = target.id();
        int a_depth = arena_.depth(a), b_depth = arena_.depth(b);
        down_.clear();
        while (a_depth > b_depth) {
            work_->undo(arena_.action(a)); a = arena_.parent(a); --a_depth;
        }
        while (b_depth > a_depth) {
            down_.push_back(b); b = arena_.parent(b); --b_depth;
        }
        while (a != b) {
            work_->undo(arena_.action(a)); a = arena_.parent(a);
            down_.push_back(b); b = arena_.parent(b);
        }
        for (auto it = down_.rbegin(); it != down_.rend(); ++it) work_->apply(arena_.action(*it));
        cursor_ = std::move(target); // The expanded candidate no longer needs to own this history.
    }
    template<class Queue>
    void expand(const Ptr& node, Queue& next) {
        const int depth = arena_.depth(node->trace.id()) + 1;
        const Handle& parent = Tree ? cursor_ : node->trace;
        // Borrow callback arguments until admission is known. In particular,
        // moving a large inline State by value would copy it even on rejection.
        auto emit = [&](auto&& state, const Action& action, auto&& info) {
            // Expansion limits apply between nodes, not inside an expansion.
            if (timed_out()) return;
            ++result_.stats.generated;
            bool terminal = info.finished || depth == config_.max_depth;
            bool save = improves(info, depth, terminal);
            bool retain = !terminal && next.prepare(info, result_.stats);
            if (!save && !retain) return;
            auto trace = arena_.child(parent, action);
            if (save) {
                auto& slot = terminal ? best_ : partial_;
                slot = Saved{info.cost, depth, trace};
            }
            if (retain) next.insert_prepared(make_node(std::forward<decltype(info)>(info),
                std::move(trace), std::forward<decltype(state)>(state), serial_++));
            if (terminal && config_.stop_on_first_solution) {
                stopped_ = true;
                result_.stop_reason = SearchStopReason::solution_found;
            }
        };
        if constexpr (Tree) {
            move_to(node->trace);
            if (timed_out()) return;
            ++result_.stats.expanded;
            work_->expand(node->info, [&](const Action& action, auto&& info) {
                emit(std::monostate{}, action, std::forward<decltype(info)>(info));
            });
        } else {
            ++result_.stats.expanded;
            static_cast<const State&>(node->state).expand(node->info, emit);
        }
        timed_out();
    }
    Result finish(SearchStopReason reason) {
        if (!stopped_) result_.stop_reason = reason;
        if (best_) result_.best.emplace(SearchPath<Action, Cost>{arena_.path(best_->trace), best_->cost, best_->depth});
        if (partial_) result_.partial.emplace(SearchPath<Action, Cost>{arena_.path(partial_->trace), partial_->cost, partial_->depth});
        result_.stats.elapsed_sec = elapsed();
        return std::move(result_);
    }
public:
    Run(const State& state, const Info& info, const SearchConfig& config)
        : initial_(state), initial_info_(info), config_(config),
          root_(arena_.root()), cursor_(root_) {
        assert(config.max_depth >= 0);
        assert(config.time_check_interval > 0);
        assert(!config.time_limit_sec ||
               (std::isfinite(*config.time_limit_sec) && *config.time_limit_sec >= 0));
        if constexpr (Tree) work_.emplace(state);
    }
    template<class Policy>
    Result fixed(Policy policy) {
        validate(policy);
        if (consider(initial_info_, root_))
            return finish(config_.stop_on_first_solution ? SearchStopReason::solution_found :
                (config_.max_depth == 0 ? SearchStopReason::depth_limit : SearchStopReason::frontier_empty));
        using Selector = std::conditional_t<std::is_same_v<Policy, FixedBeam>,
            FixedSelector<Node>, std::conditional_t<std::is_same_v<Policy,KeyedBeam>,
                KeyedSelector<Node>,Frontier<Node, Policy, Keyed>>>;
        Selector current(policy, config_.deduplicate, candidate_pool_, index_pool_),
                 next(policy, config_.deduplicate, candidate_pool_, index_pool_);
        current.insert(root_node(), result_.stats);
        std::vector<typename Selector::Ptr> batch;
        for (int depth = 0; depth < config_.max_depth; ++depth) {
            current.drain(batch, Tree && config_.tree_dfs_order);
            for (auto& node : batch) {
                if (!budget()) return finish(SearchStopReason::frontier_empty);
                expand(node, next);
                node.reset();
                if (stopped_) return finish(SearchStopReason::frontier_empty);
            }
            if (depth == config_.max_depth - 1) return finish(SearchStopReason::depth_limit);
            if (next.empty()) return finish(SearchStopReason::frontier_empty);
            std::swap(current, next);
        }
        return finish(SearchStopReason::depth_limit);
    }
    template<class Expand, class Limit>
    Result repeated(Chokudai<Expand, Limit> policy) {
        validate(policy.expand_per_depth);
        validate(policy.frontier_limit);
        if (consider(initial_info_, root_))
            return finish(config_.stop_on_first_solution ? SearchStopReason::solution_found :
                (config_.max_depth == 0 ? SearchStopReason::depth_limit : SearchStopReason::frontier_empty));
        using Queue = Frontier<Node, Limit, Keyed>;
        std::vector<std::unique_ptr<Queue>> queues;
        auto queue_at = [&](std::size_t depth) -> Queue& {
            while (queues.size() <= depth)
                queues.push_back(std::make_unique<Queue>(policy.frontier_limit, config_.deduplicate, candidate_pool_, index_pool_));
            return *queues[depth];
        };
        queue_at(0).insert(root_node(), result_.stats);
        std::vector<Ptr> batch;
        for (;;) {
            for (std::size_t depth = 0; depth < queues.size(); ++depth) {
                if (queues[depth]->empty()) continue;
                if (!budget()) return finish(SearchStopReason::frontier_empty);
                queues[depth]->take(policy.expand_per_depth, batch);
                for (auto& node : batch) {
                    if (!budget()) return finish(SearchStopReason::frontier_empty);
                    // Last-depth children are terminal; the destination stays empty.
                    expand(node, queue_at(depth + 1));
                    node.reset();
                    if (stopped_) return finish(SearchStopReason::frontier_empty);
                }
            }
            bool empty = true;
            for (const auto& queue : queues) if (!queue->empty()) { empty = false; break; }
            if (empty) return finish(SearchStopReason::frontier_empty);
        }
    }
};

template<class State, class Action, class Info, bool Tree>
class Search {
    State initial_;
    Info initial_info_;
public:
    Search(const State& state, const Info& info) : initial_(state), initial_info_(info) {}
    template<class Policy, std::enable_if_t<is_selection<Policy>, int> = 0>
    auto run(Policy policy, const SearchConfig& config) const {
        return Run<State, Action, Info, Tree, uses_key<Policy>>(initial_, initial_info_, config).fixed(policy);
    }
    template<class Expand, class Limit>
    auto run(Chokudai<Expand, Limit> policy, const SearchConfig& config) const {
        static_assert(is_selection<Expand>, "Invalid chokudai expansion policy");
        static_assert(is_selection<Limit> || std::is_same_v<Limit, Unbounded>, "Invalid chokudai frontier policy");
        return Run<State, Action, Info, Tree, uses_key<Expand> || uses_key<Limit>>(
            initial_, initial_info_, config).repeated(std::move(policy));
    }
};
} // namespace beam_search_detail

template<class State, class Action, class Info>
using CopySearch = beam_search_detail::Search<State, Action, Info, false>;
template<class State, class Action, class Info>
using TreeSearch = beam_search_detail::Search<State, Action, Info, true>;
