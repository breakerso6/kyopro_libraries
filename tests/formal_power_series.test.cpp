#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include <bits/stdc++.h>
#include <atcoder/modint>
#include "libraries/math/FormalPowerSeries.hpp"

using namespace std;
using mint = atcoder::modint998244353;
using FPS = FormalPowerSeries<mint>;

static vector<mint> naive_mul(const vector<mint>& a, const vector<mint>& b, int n = -1) {
    if (a.empty() || b.empty()) return {};
    vector<mint> c(a.size() + b.size() - 1);
    for (int i = 0; i < (int)a.size(); ++i) for (int j = 0; j < (int)b.size(); ++j) c[i + j] += a[i] * b[j];
    if (n >= 0 && (int)c.size() > n) c.resize(n);
    return c;
}

static vector<mint> naive_inv(const vector<mint>& f, int n) {
    vector<mint> g(n);
    g[0] = mint(1) / f[0];
    for (int i = 1; i < n; ++i) {
        mint sum = 0;
        for (int j = 1; j <= i && j < (int)f.size(); ++j) sum += f[j] * g[i - j];
        g[i] = -sum / f[0];
    }
    return g;
}

static vector<mint> naive_log(const vector<mint>& f, int n) {
    FPS fp(f);
    return (fp.diff() * FPS(naive_inv(f, n))).pre(n - 1).integral().pre(n).vec();
}

static vector<mint> naive_exp(const vector<mint>& f, int n) {
    vector<mint> g(n);
    g[0] = 1;
    for (int i = 1; i < n; ++i) {
        mint sum = 0;
        for (int j = 1; j <= i; ++j) sum += mint(j) * (j < (int)f.size() ? f[j] : mint()) * g[i - j];
        g[i] = sum / mint(i);
    }
    return g;
}

static void test_basic_operations() {
    FPS f{1, 2, 3}, g{3, 4};
    assert(((f + g).vec() == vector<mint>{4, 6, 3}));
    assert(((f - g).vec() == vector<mint>{-2, -2, 3}));
    assert(((f * g).vec() == vector<mint>{3, 10, 17, 12}));
    assert((f.diff().vec() == vector<mint>{2, 6}));
    assert((f.diff().integral().vec() == vector<mint>{0, 2, 3}));
    assert(f.eval(2) == mint(17));

    FPS p{2, 3, 4, 5};
    FPS q{1, 2};
    auto [quo, rem] = p.divmod(q);
    assert(p == q * quo + rem);
    assert(rem.size() < q.size());
}

static void test_fps_functions() {
    vector<mint> f{1, 2, 5, 7, 11, 13, 17, 19};
    FPS fp(f);
    assert((fp.inv(8).vec() == naive_inv(f, 8)));
    assert(((fp * fp.inv(8)).pre(8).vec() == vector<mint>{1, 0, 0, 0, 0, 0, 0, 0}));
    assert((fp.log(8).vec() == naive_log(f, 8)));
    assert(fp.log(8).exp(8).vec() == fp.pre(8).vec());

    vector<mint> h{0, 3, 2, 9, 4, 8, 1, 7};
    assert((FPS(h).exp(8).vec() == naive_exp(h, 8)));
    assert(FPS(h).exp(8).log(8).vec() == FPS(h).pre(8).vec());

    FPS shifted{0, 0, 2, 3, 1};
    auto cube = shifted.pow(3, 12);
    assert(cube.vec() == (shifted * shifted * shifted).pre(12).vec());
    assert((shifted.pow(0, 5).vec() == vector<mint>{1, 0, 0, 0, 0}));
}

static void test_large_convolution_path() {
    vector<mint> a(90), b(85);
    for (int i = 0; i < (int)a.size(); ++i) a[i] = i * i + 3;
    for (int i = 0; i < (int)b.size(); ++i) b[i] = 5 * i + 1;
    assert((FPS(a) * FPS(b)).vec() == naive_mul(a, b));
}

int main() {
    test_basic_operations();
    test_fps_functions();
    test_large_convolution_path();
    cout << "Hello World\n";
}
