#pragma once
#include <bits/stdc++.h>

struct RollbackDSU {
    std::vector<int> data;
    std::vector<std::pair<int, int>> history;
    int components;

    explicit RollbackDSU(int n) : data(n, -1), components(n) {}

    int leader(int v) const {
        while (data[v] >= 0) v = data[v];
        return v;
    }
    bool same(int a, int b) const { return leader(a) == leader(b); }
    int size(int v) const { return -data[leader(v)]; }
    int snapshot() const { return (int)history.size(); }

    bool merge(int a, int b) {
        a = leader(a); b = leader(b);
        if (a == b) return false;
        if (-data[a] < -data[b]) std::swap(a, b);
        history.push_back({a, data[a]});
        history.push_back({b, data[b]});
        data[a] += data[b];
        data[b] = a;
        --components;
        return true;
    }
    bool undo() {
        if (history.empty()) return false;
        auto [b, db] = history.back(); history.pop_back();
        auto [a, da] = history.back(); history.pop_back();
        data[a] = da; data[b] = db;
        ++components;
        return true;
    }
    void rollback(int state) {
        while ((int)history.size() > state) undo();
    }
};
