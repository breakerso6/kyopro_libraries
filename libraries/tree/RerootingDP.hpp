#include<bits/stdc++.h>
#include<atcoder/modint>
using namespace std;
using mint = atcoder::modint;


template <class E, class V, E (*merge)(E, E), E (*e)(), E (*put_edge)(V, int), V (*put_vertex)(E, int)>
struct RerootingDP {
    struct edge {
        int to, idx, xdi;
    };
    
    // MODIFIED: コンストラクタを改造し、頂点ごとの初期値を受け取れるようにする
    RerootingDP(int n_ = 0, vector<E> initial_values_ = {}) : n(n_), inner_edge_id(0) {
        es.resize(2*n-2);
        start.resize(2*n-2);
        if (initial_values_.empty()){
            // 初期値が指定されなかった場合、従来通り e() で初期化
            initial_values.assign(n, e());
        } else {
            // 初期値が指定された場合、サイズをチェックして move で受け取る
            assert((int)initial_values_.size() == n_);
            initial_values = move(initial_values_);
        }
        if (n == 1) es_build();
    }

    void add_edge(int u, int v, int idx, int xdi){
        start[inner_edge_id] = u;
        es[inner_edge_id] = {v,idx,xdi};
        inner_edge_id++;
        start[inner_edge_id] = v;
        es[inner_edge_id] = {u,xdi,idx};
        inner_edge_id++;
        if (inner_edge_id == 2*n-2){
            es_build();
        }
    }

    vector<V> build(int root_ = 0){
        root = root_;
        vector<V> subdp(n); // subdp[0] = put_vertex(e(),0); は不要なため削除
        outs.resize(n);
        vector<int> geta(n+1,0);
        for (int i = 0; i < n; i++) geta[i+1] = start[i+1] - start[i] - 1;
        geta[root+1]++;
        for (int i = 0; i < n; i++) geta[i+1] += geta[i];
        
        auto dfs = [&](auto sfs, int v, int f) -> void {
            // MODIFIED: 単位元 e() の代わりに、頂点 v の初期値 initial_values[v] から計算を始める
            E val = initial_values[v]; 
            for (int i = start[v]; i < start[v+1]; i++){
                if (es[i].to == f){
                    swap(es[start[v+1]-1],es[i]);
                }
                if (es[i].to == f) continue;
                sfs(sfs,es[i].to,v);
                E nval = put_edge(subdp[es[i].to],es[i].idx);
                outs[geta[v]++] = nval;
                val = merge(val,nval);
            }
            subdp[v] = put_vertex(val, v);
        };
        dfs(dfs,root,-1);
        return subdp;
    }

    vector<V> reroot(){
        vector<E> reverse_edge(n);
        reverse_edge[root] = e();
        vector<V> answers(n);
        auto dfs = [&](auto sfs, int v) -> void {
            int le = outs_start(v);
            int ri = outs_start(v+1);
            int siz = ri - le;
            vector<E> rui(siz+1);
            rui[siz] = e();
            for (int i = siz-1; i >= 0; i--){
                rui[i] = merge(outs[le+i],rui[i+1]);
            }

            // MODIFIED: 全方位からの情報をマージした後、さらに頂点vの初期値をマージする
            E all_merged = merge(rui[0], reverse_edge[v]);
            answers[v] = put_vertex(merge(initial_values[v], all_merged), v);
            
            E lui = e();
            for (int i = 0; i < siz; i++){
                // MODIFIED: 子に渡す情報にも、頂点vの初期値を反映させる
                E merged_for_child = merge(merge(lui, rui[i+1]), reverse_edge[v]);
                V rdp = put_vertex(merge(initial_values[v], merged_for_child), v);
                
                reverse_edge[es[start[v]+i].to] = put_edge(rdp,es[start[v]+i].xdi);
                lui = merge(lui,outs[le+i]);
                sfs(sfs,es[start[v]+i].to);
            }
        };
        dfs(dfs,root);
        return answers;
    }

private:
    int n, root, inner_edge_id;
    vector<E> outs;
    vector<edge> es;
    vector<int> start;
    
    // ADDED: 頂点ごとの初期値を保持するメンバ変数
    vector<E> initial_values;

    int outs_start(int v){
        int res = start[v] - v;
        if (root < v) res++;
        return res;
    }
    void es_build(){
        vector<edge> nes(2*n-2);
        vector<int> nstart(n+2,0);
        for (int i = 0; i < 2*n-2; i++) nstart[start[i]+2]++;
        for (int i = 0; i < n; i++) nstart[i+1] += nstart[i];
        for (int i = 0; i < 2*n-2; i++) nes[nstart[start[i]+1]++] = es[i];
        swap(es,nes);
        swap(start,nstart);
    }
};