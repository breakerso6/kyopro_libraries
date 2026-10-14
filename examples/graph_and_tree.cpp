#include <iostream>
#include "libraries/graph/GraphAlgorithms.hpp"
#include "libraries/tree/Tree.hpp"

int main() {
    Graph<long long, true> directed(4);
    directed.add_edge(0, 1, 2);
    directed.add_edge(1, 2, 3);
    directed.add_edge(0, 2, 8);
    directed.add_edge(2, 3, 1);
    auto shortest = dijkstra(directed, 0, 4000000000000000000LL);
    std::cout << "shortest: " << shortest.distance[3] << '\n'; // 6
    auto path = shortest.path_to(3);
    if (path) {
        std::cout << "vertices:";
        for (int v : path->vertices) std::cout << ' ' << v;
        std::cout << '\n'; // 0 1 2 3
    }
    directed.add_edge(3, 1);
    auto scc = strongly_connected_components(directed);
    std::cout << "SCC: " << scc.size() << '\n'; // 2

    Tree<> tree(5);
    tree.add_edge(0, 1, 3);
    tree.add_edge(1, 2, 4);
    tree.add_edge(1, 3, 5);
    tree.add_edge(3, 4, 6);
    tree.build(0);
    std::cout << "LCA: " << tree.lca(2, 4) << '\n'; // 1
    std::cout << "cost: " << *tree.weighted_distance(2, 4) << '\n'; // 15
    auto segments = tree.path_segments(2, 4);
    if (segments) {
        std::cout << "tree path:";
        for (auto segment : *segments) {
            if (segment.reversed)
                for (int i = segment.right - 1; i >= segment.left; --i) std::cout << ' ' << tree.vertex[i];
            else
                for (int i = segment.left; i < segment.right; ++i) std::cout << ' ' << tree.vertex[i];
        }
        std::cout << '\n'; // 2 1 3 4
    }
    std::cout << "diameter: " << tree.diameter()->length << '\n'; // 15
}
