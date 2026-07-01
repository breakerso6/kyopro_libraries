#include <bits/stdc++.h>
#include <atcoder/modint>
#include "libraries/math/IntegerMath.hpp"
#include "libraries/math/ModCombinatorics.hpp"

using namespace std;
using namespace integer_math;

long long brute_floor_sum(long long n, long long m, long long a, long long b) {
    long long result = 0;
    for (long long i = 0; i < n; ++i) {
        result += floor_div(a * i + b, m);
    }
    return result;
}

void test_integer_math() {
    assert(safe_mod(-1LL, 5LL) == 4);
    assert(safe_mod(6LL, 5LL) == 1);

    assert(floor_div(3LL, 2LL) == 1);
    assert(floor_div(-3LL, 2LL) == -2);
    assert(floor_div(3LL, -2LL) == -2);
    assert(floor_div(-3LL, -2LL) == 1);
    assert(ceil_div(3LL, 2LL) == 2);
    assert(ceil_div(-3LL, 2LL) == -1);
    assert(ceil_div(3LL, -2LL) == -1);
    assert(ceil_div(-3LL, -2LL) == 2);

    assert(gcd(-12LL, 18LL) == 6);
    assert(lcm(12LL, 18LL) == 36);
    assert(lcm(-12LL, 18LL) == 36);

    auto eg = ext_gcd(30, 18);
    assert(eg.gcd == 6);
    assert(30 * eg.x + 18 * eg.y == eg.gcd);

    assert(inv_mod(3, 11) == 4);
    assert(pow_mod(2, 10, 1000) == 24);

    assert(crt({2, 3}, {3, 5}) == make_pair(8LL, 15LL));
    assert(crt({1, 2}, {2, 4}) == make_pair(0LL, 0LL));

    for (long long n = 0; n <= 12; ++n) {
        for (long long m = 1; m <= 10; ++m) {
            for (long long a = -10; a <= 10; ++a) {
                for (long long b = -10; b <= 10; ++b) {
                    assert(floor_sum(n, m, a, b) == brute_floor_sum(n, m, a, b));
                }
            }
        }
    }
}

void test_mod_combinatorics() {
    using mint = atcoder::modint998244353;
    ModCombinatorics<mint> mc(5);
    assert(mc.factorial(5).val() == 120);
    assert(mc.inv_factorial(5) * mc.factorial(5) == mint(1));
    assert(mc.inverse(5).val() == mint(5).inv().val());
    assert(mc.perm(5, 2).val() == 20);
    assert(mc.comb(5, 2).val() == 10);
    assert(mc.homo(3, 2).val() == 6);
    assert(mc.multinomial(vector<int>{2, 3, 4}).val() == 1260);
    assert(mc.multinomial({1, 2, 2}).val() == 30);
    assert(mc.multinomial(5, vector<int>{1, 2, 2}).val() == 30);
    assert(mc.multinomial(6, vector<int>{1, 2, 2}).val() == 0);
    assert(mc.multinomial(vector<int>{1, -1}).val() == 0);
    assert(mc.catalan(0).val() == 1);
    assert(mc.catalan(4).val() == 14);
    assert(mc.derangement(0).val() == 1);
    assert(mc.derangement(1).val() == 0);
    assert(mc.derangement(4).val() == 9);
    assert(mc.comb(3, 4).val() == 0);
    assert(mc.perm(3, -1).val() == 0);
    assert(mc.factorial(8).val() == 40320);

    using dmint = atcoder::dynamic_modint<0>;
    dmint::set_mod(1000000007);
    ModCombinatorics<dmint> dmc(10);
    assert(dmc.comb(10, 3).val() == 120);
    assert(dmc.perm(10, 3).val() == 720);
    assert(dmc.multinomial({2, 3, 5}).val() == 2520);
    assert(dmc.catalan(5).val() == 42);
    assert(dmc.derangement(5).val() == 44);
}

int main() {
    test_integer_math();
    test_mod_combinatorics();
    return 0;
}
