// #pragma GCC optimize("Ofast")
#include <bits/stdc++.h>
// #include <atcoder/all>
// using namespace atcoder;
using namespace std;

using ll = long long;
using ull = unsigned long long;
using vecint = std::vector<int>;
using vecll = std::vector<long long>;
using vecstr = std::vector<string>;
using vecbool = std::vector<bool>;
using vecdou = std::vector<double>;
using vecpl = std::vector<pair<ll,ll>>;
using vec2d = std::vector<vecll>;
using vec2di = std::vector<vecint>;
using vec2dd = std::vector<vecdou>;
using vec2db = std::vector<vecbool>;
using pl = pair<long long,long long>;
// using mint998 = modint998244353;
// using mint107 = modint1000000007;
// using mint = modint;
// using vecmint = std::vector<mint>;
// using vecmint998 = std::vector<mint998>;
// using vecmint107 = std::vector<mint107>;
// using vec2dmint = std::vector<vecmint>; 
// using vec2dmint998 = std::vector<vecmint998>;
// using vec2dmint107 = std::vector<vecmint107>;
#define rep(i,n) for (ll i = 0; i < (ll)(n); i++)
#define rep1(i,n) for (ll i = 1; i <= (ll)(n); i++)
#define REP(i,l,r) for (ll i = (ll)(l); i < (ll)(r); i++)
#define rrep(i,n) for (ll i = (ll)(n)-1; i >= 0; i--)
#define rrep1(i,n) for (ll i = (ll)(n); i > 0; i--)
#define RREP(i,l,r) for (ll i = (ll)(r)-1; i >= (ll)(l); i--)
#define all(a) (a).begin(), (a).end()
#define INF ((1LL<<62)-(1LL<<31))
#define inr(a,x,b) ((a) <= (x) && (x) < (b))
template <typename T>
bool chmax(T &a, const T &b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}
template <typename T>
bool chmin(T &a, const T &b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}

void ynout(bool x,string Tru="Yes",string Wro="No"){
    if(x){
        cout << Tru << '\n';
    }else{
        cout << Wro << '\n';
    }
}
ll power(ll a,ll b,ll mod=INF){
    long long x=1,y=a%mod;
    while(b>0){
        if(b&1ll){
            x=(x*y)%mod;
        }
        y=(y*y)%mod;
        b>>=1;
    }
  return x%mod;
}
ll Pdist2(pair<ll,ll> a,pair<ll,ll> b){
    return (a.first-b.first)*(a.first-b.first)+(a.second-b.second)*(a.second-b.second);
}
double Pdist(pair<ll,ll> a,pair<ll,ll> b){
    return sqrt(Pdist2(a,b));
}
ll PdistM(pair<ll,ll> a,pair<ll,ll> b){
    return abs(a.first-b.first)+abs(a.second-b.second);
}
ll gcd(ll a,ll b){
    if(b==0){
        return a;
    }else{
        return gcd(b,a%b);
    }
}
ll lcm(ll a,ll b){
    return a/gcd(a,b)*b;
}
template <typename T>
void print(const std::vector<T>& v) {
    for (const auto& elem : v) {
        cout << elem << " ";
    }
    cout << '\n';
}
template <typename T>
void print2d(const std::vector<std::vector<T>>& v) {
    for (const auto& row : v) {
        for (const auto& elem : row) {
            cout << elem << " ";
        }
        cout << '\n';
    }
}
vecll vecinp(ll n){
    vecll v(n);
    rep(i,n) cin >> v[i];
    return v;
}

// vecll dx = {1,0,-1,0};
// vecll dy = {0,1,0,-1};
// vector<char> dir = {'D','R','U','L'};

void solve(){
    
}

int main() {
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
    ll t=1;
    // std::cin >> t;
    rep(i,t)solve();
}