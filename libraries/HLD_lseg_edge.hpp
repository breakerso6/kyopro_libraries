#include <bits/stdc++.h>
#include <atcoder/lazysegtree>
#include "edge.hpp"
using namespace std;
using ll = long long;
using vecll = std::vector<long long>;
#define rep(i,n) for (ll i = 0; i < (ll)(n); i++)

template <class S,
          S (*op)(S, S),
          S (*e)(),
          class F,
          S (*mapping)(F, S),
          F (*composition)(F, F),
          F (*id_)()>
struct HLD_lseg_edge {
    vecll vertex;
    vecll id;
    vecll head;
    vecll parent;
    vecll depth;
    vecll subsize;
    vecll heavy_child;
    int root;

    static S op_rev(S a, S b) {
        return op(b, a);
    }
    atcoder::lazy_segtree<S, op, e, F, mapping, composition, id_> seg;
    atcoder::lazy_segtree<S, op_rev, e, F, mapping, composition, id_> seg_rev;

    vecll edge_id;

    HLD_lseg_edge(int n, const vector<edge<S>>& edges, int root_ = 0) {
        root = root_;
        vertex.resize(n);
        id.resize(n);
        head.resize(n);
        parent.resize(n);
        depth.resize(n);
        subsize.resize(n);
        heavy_child.resize(n);
        seg = atcoder::lazy_segtree<S, op, e, F, mapping, composition, id_>(n);
        seg_rev = atcoder::lazy_segtree<S, op_rev, e, F, mapping, composition, id_>(n);
        edge_id.resize(n-1);
        vector<vector<ll>> graph(n);
        for (const auto& edge : edges) {
            int u = edge.from, v = edge.to;
            graph[u].emplace_back(v);
            graph[v].emplace_back(u);
        }
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
            };
            dfs(root, root);
        }
        rep(i,edges.size()) {
            const auto& edge = edges[i];
            int u = edge.from, v = edge.to;
            S w = edge.weight;
            if (parent[v] == u) {
                seg.set(id[v], w);
                seg_rev.set(id[v], w);
                edge_id[i] = id[v];
            } else {
                seg.set(id[u], w);
                seg_rev.set(id[u], w);
                edge_id[i] = id[u];
            }
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

    // s->tのパス上i番目の頂点を返す
    int jump(int s, int t, int i) {
        int l = lca(s, t);
        if (i <= depth[s] - depth[l]) {
            return level_ancestor(s, depth[s] - i);
        } else {
            return level_ancestor(t, i - depth[s]  + 2*depth[l]);
        }
    }

    // vの値をxに更新
    void set(int v, S x) {
        seg.set(edge_id[v], x);
        seg_rev.set(edge_id[v], x);
    }

    S get(int i) {
        return seg.get(edge_id[i]);
    }


    // s->tのパス(e0,...,ek)に対し、e0・...・ekを返す
    S prod_path(int s, int t) {
        int l = lca(s, t);
        S res_left = e(), res_right = e();
        while (head[s] != head[l]) {
            res_left = op(res_left, seg_rev.prod(id[head[s]], id[s] + 1));
            s = parent[head[s]];
        }
        res_left = op(res_left, seg_rev.prod(id[l] + 1, id[s] + 1));
        while (head[t] != head[l]) {
            res_right = op(seg.prod(id[head[t]], id[t] + 1), res_right);
            t = parent[head[t]];
        }
        res_right = op(seg.prod(id[l] + 1, id[t] + 1), res_right);
        return op(res_left, res_right);
    }

    void apply_path(int s, int t, F f) {
        int l = lca(s, t);
        while (head[s] != head[l]) {
            seg.apply(id[head[s]], id[s] + 1, f);
            seg_rev.apply(id[head[s]], id[s] + 1, f);
            s = parent[head[s]];
        }
        seg.apply(id[l] + 1, id[s] + 1, f);
        seg_rev.apply(id[l] + 1, id[s] + 1, f);
        while (head[t] != head[l]) {
            seg.apply(id[head[t]], id[t] + 1, f);
            seg_rev.apply(id[head[t]], id[t] + 1, f);
            t = parent[head[t]];
        }
        seg.apply(id[l] + 1, id[t] + 1, f);
        seg_rev.apply(id[l] + 1, id[t] + 1, f);
    }
};
