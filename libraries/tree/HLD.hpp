#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using vecll = std::vector<long long>;
#define rep(i,n) for (ll i = 0; i < (ll)(n); i++)

struct HLD {
    vecll vertex;
    vecll id;
    vecll head;
    vecll parent;
    vecll depth;
    vecll subsize;
    vecll heavy_child;
    vecll out;
    int root;

    HLD(const vector<vector<ll>>& graph, int root_ = 0) {
        root = root_;
        int n = graph.size();
        vertex.resize(n);
        id.resize(n);
        head.resize(n);
        parent.resize(n);
        depth.resize(n);
        subsize.resize(n);
        heavy_child.resize(n);
        out.resize(n);
        {
            function<void(int,int,int)> dfs = [&](int v, int p, int d) {
                parent[v] = p;
                depth[v] = d;
                subsize[v] = 1;
                heavy_child[v] = -1;
                int max_subsize = 0;
                for (int to : graph[v]) {
                    if (to == p) continue;
                    dfs(to, v, d + 1);
                    subsize[v] += subsize[to];
                    if (subsize[to] > max_subsize) {
                        max_subsize = subsize[to];
                        heavy_child[v] = to;
                    }
                }
            };
            dfs(root, -1, 0);
        }
        {
            int idx = 0;
            function<void(int,int)> dfs = [&](int v, int h) {
                head[v] = h;
                id[v] = idx;
                vertex[idx] = v;
                idx++;
                if (heavy_child[v] != -1) {
                    dfs(heavy_child[v], h);
                }
                for (int to : graph[v]) {
                    if (to == parent[v] || to == heavy_child[v]) continue;
                    dfs(to, to);
                }
                out[v] = idx;
            };
            dfs(root, root);
        }
    }

    // vの祖先で深さがdのものを返す
    int level_ancestor(int v, int d) {
        if (depth[v] < d) return -1;
        while (depth[head[v]] > d) {
            v = parent[head[v]];
        }
        return vertex[id[v] - (depth[v] - d)];
    }

    // uとvのLCAを返す
    int lca(int u, int v) {
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                u = parent[head[u]];
            } else {
                v = parent[head[v]];
            }
        }
        return depth[u] < depth[v] ? u : v;
    }

    // uとvの距離を返す
    int distance(int u, int v) {
        int l = lca(u, v);
        return depth[u] + depth[v] - 2 * depth[l];
    }

    bool in_subtree(int ancestor, int v) const {
        return id[ancestor] <= id[v] && id[v] < out[ancestor];
    }

    vector<pair<int,int>> path_segments(int u, int v, bool vertex_query = true) {
        vector<pair<int,int>> left, right;
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                left.push_back({(int)id[head[u]], (int)id[u] + 1});
                u = parent[head[u]];
            } else {
                right.push_back({(int)id[head[v]], (int)id[v] + 1});
                v = parent[head[v]];
            }
        }
        int l = min(id[u], id[v]) + (vertex_query ? 0 : 1);
        int r = max(id[u], id[v]) + 1;
        if (l < r) left.push_back({l, r});
        reverse(right.begin(), right.end());
        left.insert(left.end(), right.begin(), right.end());
        return left;
    }

    // s->tのパス上i番目の頂点を返す
    int jump(int s, int t, int i) {
        int l = lca(s, t);
        if (i <= depth[s] - depth[l]) {
            return level_ancestor(s, depth[s] - i);
        } else {
            return level_ancestor(t, i - depth[s]  + 2*depth[l]);
        }
    }
};
