#pragma once
#include <bits/stdc++.h>

struct HopcroftKarp {
    int left_size, right_size;
    std::vector<std::vector<int>> graph;
    std::vector<int> match_left, match_right, distance;

    HopcroftKarp(int left_size_, int right_size_)
        : left_size(left_size_), right_size(right_size_), graph(left_size_),
          match_left(left_size_, -1), match_right(right_size_, -1), distance(left_size_) {}
    void add_edge(int left, int right) { graph[left].push_back(right); }
    int max_matching() {
        int result = 0;
        for (int v : match_left) result += v != -1;
        while (bfs())
            for (int v = 0; v < left_size; ++v) if (match_left[v] == -1 && dfs(v)) ++result;
        return result;
    }
    // Call after max_matching(). Returns {left vertices, right vertices}.
    std::pair<std::vector<int>, std::vector<int>> minimum_vertex_cover() const {
        std::vector<char> seen_left(left_size), seen_right(right_size);
        std::queue<int> q;
        for (int v = 0; v < left_size; ++v) if (match_left[v] == -1) seen_left[v] = 1, q.push(v);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (int to : graph[v]) if (match_left[v] != to && !seen_right[to]) {
                seen_right[to] = 1;
                int next = match_right[to];
                if (next != -1 && !seen_left[next]) seen_left[next] = 1, q.push(next);
            }
        }
        std::vector<int> left_cover, right_cover;
        for (int v = 0; v < left_size; ++v) if (!seen_left[v]) left_cover.push_back(v);
        for (int v = 0; v < right_size; ++v) if (seen_right[v]) right_cover.push_back(v);
        return {left_cover, right_cover};
    }

private:
    bool bfs() {
        std::queue<int> q; bool reachable = false;
        std::fill(distance.begin(), distance.end(), -1);
        for (int v = 0; v < left_size; ++v) if (match_left[v] == -1) distance[v] = 0, q.push(v);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (int to : graph[v]) {
                int next = match_right[to];
                if (next == -1) reachable = true;
                else if (distance[next] == -1) distance[next] = distance[v] + 1, q.push(next);
            }
        }
        return reachable;
    }
    bool dfs(int v) {
        for (int to : graph[v]) {
            int next = match_right[to];
            if (next == -1 || (distance[next] == distance[v] + 1 && dfs(next))) {
                match_left[v] = to; match_right[to] = v; return true;
            }
        }
        distance[v] = -1; return false;
    }
};
