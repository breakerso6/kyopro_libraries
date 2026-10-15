// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/graph/HopcroftKarp.hpp
// SPDX-License-Identifier: CC0-1.0

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
        int upper_bound = std::min(left_size, right_size);
        if (result == upper_bound) return result;
        // This is the same direct augmentation done by the first BFS layer,
        // but avoids scanning every edge to build that layer first.
        for (int v = 0; v < left_size && result < upper_bound; ++v) if (match_left[v] == -1)
            for (int to : graph[v]) if (match_right[to] == -1) {
                match_left[v] = to; match_right[to] = v; ++result;
                break;
            }
        if (result == upper_bound) return result;
        cursor.resize(left_size);
        queue_buffer.resize(left_size);
        while (bfs()) {
            std::fill(cursor.begin(), cursor.end(), 0);
            for (int v = 0; v < left_size; ++v) if (match_left[v] == -1 && dfs(v)) ++result;
            if (result == upper_bound) break;
        }
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
    std::vector<int> cursor, queue_buffer;
    int shortest = 0;
    bool bfs() {
        int front = 0, back = 0;
        shortest = left_size + 1;
        std::fill(distance.begin(), distance.end(), -1);
        for (int v = 0; v < left_size; ++v) if (match_left[v] == -1) distance[v] = 0, queue_buffer[back++] = v;
        while (front < back) {
            int v = queue_buffer[front++];
            if (distance[v] >= shortest) continue;
            for (int to : graph[v]) {
                int next = match_right[to];
                if (next == -1) shortest = distance[v] + 1;
                else if (distance[next] == -1) distance[next] = distance[v] + 1, queue_buffer[back++] = next;
            }
        }
        return shortest <= left_size;
    }
    bool dfs(int v) {
        for (int& i = cursor[v]; i < (int)graph[v].size(); ++i) {
            int to = graph[v][i];
            int next = match_right[to];
            if ((next == -1 && distance[v] + 1 == shortest) ||
                (next != -1 && distance[v] + 1 < shortest && distance[next] == distance[v] + 1 && dfs(next))) {
                match_left[v] = to; match_right[to] = v; return true;
            }
        }
        distance[v] = -1; return false;
    }
};
