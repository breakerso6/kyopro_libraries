#include <iostream>
#include "libraries/tree/Tree.hpp"

// Input:
// n q
// a[1] ... a[n]
// n-1 lines: u v (1-indexed)
// q queries: 0 v delta (subtree add), 1 v (subtree sum)
// The root is input vertex 1. Each subtree includes its root vertex.
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    if (!(std::cin >> n >> q)) return 0;
    std::vector<long long> initial(n);
    for (auto& value : initial) std::cin >> value;
    Tree<> tree(n);
    tree.read_edges();
    tree.build(0);
    auto values = tree.make_add_sum(initial);
    while (q--) {
        int type, v;
        std::cin >> type >> v;
        --v;
        if (type == 0) {
            long long delta;
            std::cin >> delta;
            values.add_subtree(v, delta);
        } else {
            std::cout << values.sum_subtree(v) << '\n';
        }
    }
}
