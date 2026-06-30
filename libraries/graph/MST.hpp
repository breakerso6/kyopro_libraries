#pragma once
#include <bits/stdc++.h>

template<class T>
struct KruskalMST {
    struct Edge { int u, v; T cost; int id; };
    int n;
    std::vector<Edge> edges;
    explicit KruskalMST(int n_) : n(n_) {}
    int add_edge(int u, int v, T cost) {
        int id = (int)edges.size();
        edges.push_back({u, v, cost, id});
        return id;
    }
    std::pair<T, std::vector<Edge>> build() const {
        struct DSU {
            std::vector<int> p, s;
            DSU(int n) : p(n), s(n, 1) { std::iota(p.begin(), p.end(), 0); }
            int leader(int x) { return p[x] == x ? x : p[x] = leader(p[x]); }
            bool merge(int a, int b) {
                a = leader(a); b = leader(b);
                if (a == b) return false;
                if (s[a] < s[b]) std::swap(a, b);
                p[b] = a; s[a] += s[b]; return true;
            }
        } dsu(n);
        auto sorted = edges;
        std::sort(sorted.begin(), sorted.end(), [](const Edge& a, const Edge& b) {
            return a.cost != b.cost ? a.cost < b.cost : a.id < b.id;
        });
        T total = 0;
        std::vector<Edge> used;
        for (auto e : sorted) if (dsu.merge(e.u, e.v)) total += e.cost, used.push_back(e);
        return {total, used};
    }
};
