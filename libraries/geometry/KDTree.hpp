#pragma once
#include <bits/stdc++.h>
#include "libraries/geometry/GeometryInt.hpp"

template<class T = long long>
struct KDTree {
    using Point = geometry_int::Point<T>;
    using i128 = geometry_int::i128;
    struct Item { Point point; int id; };
    struct Node { Item item; int left = -1, right = -1; T min_x, max_x, min_y, max_y; };
    std::vector<Node> nodes;
    int root = -1;

    explicit KDTree(const std::vector<Point>& points) {
        std::vector<Item> items;
        for (int i = 0; i < (int)points.size(); ++i) items.push_back({points[i], i});
        nodes.reserve(points.size()); root = build(items, 0, (int)items.size(), 0);
    }
    std::pair<i128, int> nearest(Point query) const {
        if (root == -1) return {-1, -1};
        std::pair<i128, int> answer{(i128(1) << 126), -1};
        nearest(root, query, answer);
        return answer;
    }
    // Returns ids whose points are in [x1, x2) x [y1, y2).
    std::vector<int> range_search(T x1, T y1, T x2, T y2) const {
        std::vector<int> result;
        range_search(root, x1, y1, x2, y2, result);
        return result;
    }

private:
    int build(std::vector<Item>& items, int l, int r, int depth) {
        if (l == r) return -1;
        int m = (l + r) / 2, axis = depth & 1;
        std::nth_element(items.begin() + l, items.begin() + m, items.begin() + r, [&](const Item& a, const Item& b) {
            return axis ? a.point.y < b.point.y : a.point.x < b.point.x;
        });
        int v = (int)nodes.size(); nodes.push_back({items[m], -1, -1, items[m].point.x, items[m].point.x, items[m].point.y, items[m].point.y});
        nodes[v].left = build(items, l, m, depth + 1); nodes[v].right = build(items, m + 1, r, depth + 1);
        expand(v, nodes[v].left); expand(v, nodes[v].right);
        return v;
    }
    void expand(int v, int child) {
        if (child == -1) return;
        nodes[v].min_x = std::min(nodes[v].min_x, nodes[child].min_x); nodes[v].max_x = std::max(nodes[v].max_x, nodes[child].max_x);
        nodes[v].min_y = std::min(nodes[v].min_y, nodes[child].min_y); nodes[v].max_y = std::max(nodes[v].max_y, nodes[child].max_y);
    }
    i128 box_distance2(const Node& node, Point p) const {
        i128 dx = p.x < node.min_x ? (i128)node.min_x - p.x : p.x > node.max_x ? (i128)p.x - node.max_x : 0;
        i128 dy = p.y < node.min_y ? (i128)node.min_y - p.y : p.y > node.max_y ? (i128)p.y - node.max_y : 0;
        return dx * dx + dy * dy;
    }
    void nearest(int v, Point query, std::pair<i128, int>& answer) const {
        if (v == -1 || box_distance2(nodes[v], query) > answer.first) return;
        auto candidate = std::pair<i128, int>{geometry_int::distance2(nodes[v].item.point, query), nodes[v].item.id};
        answer = std::min(answer, candidate);
        int a = nodes[v].left, b = nodes[v].right;
        if (a != -1 && b != -1 && box_distance2(nodes[a], query) > box_distance2(nodes[b], query)) std::swap(a, b);
        nearest(a, query, answer); nearest(b, query, answer);
    }
    void range_search(int v, T x1, T y1, T x2, T y2, std::vector<int>& result) const {
        if (v == -1 || nodes[v].max_x < x1 || x2 <= nodes[v].min_x || nodes[v].max_y < y1 || y2 <= nodes[v].min_y) return;
        Point p = nodes[v].item.point;
        if (x1 <= p.x && p.x < x2 && y1 <= p.y && p.y < y2) result.push_back(nodes[v].item.id);
        range_search(nodes[v].left, x1, y1, x2, y2, result); range_search(nodes[v].right, x1, y1, x2, y2, result);
    }
};
