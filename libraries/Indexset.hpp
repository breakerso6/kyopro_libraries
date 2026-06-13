#include <bits/stdc++.h>
using namespace std;
#include "libraries/Random.hpp"
Random rnd;

struct IndexSet {
private:
    vector<int> P,pos;
    int n;
    int size_;
    void swap_pos(int i,int j){
        int x=P[i];
        int y=P[j];
        swap(P[i],P[j]);
        swap(pos[x],pos[y]);
    }
public:
    IndexSet (int n_){
        n=n_;
        size_ = n;
        P.resize(n);
        pos.resize(n);
        iota(P.begin(), P.end(), 0);
        iota(pos.begin(), pos.end(), 0);
    }
    void fill(){
        size_=n;
    }
    void clear(){
        size_ = 0;
    }
    int size(){
        return size_;
    }
    bool contain(int i){
        return pos[i]<size_;
    }
    void push(int i){
        if(contain(i))return;
        swap_pos(pos[i],size_);
        size_++;
    }
    void erase(int i){
        if(!contain(i))return;
        swap_pos(pos[i],size_-1);
        size_--;
    }
    int random(){
        if(size_==0)return -1;
        return P[rnd.nextInt(size_)];
    }
};
