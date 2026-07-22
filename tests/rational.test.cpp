#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ITP1_1_A"

#include <bits/stdc++.h>
#include "libraries/math/Rational.hpp"

using namespace std;

int main() {
    Rational<long long> a(-6, -8), b(-2, 3);
    assert(a == Rational<long long>(3, 4));
    assert(a + b == Rational<long long>(1, 12));
    assert(a - b == Rational<long long>(17, 12));
    assert(a * b == Rational<long long>(-1, 2));
    assert(a / b == Rational<long long>(-9, 8));

    Rational<long long> value(1, 3);
    value += Rational<long long>(1, 6);
    value *= Rational<long long>(-3);
    value /= Rational<long long>(2);
    value -= Rational<long long>(-1, 4);
    assert(value == Rational<long long>(-1, 2));

    assert(Rational<long long>(-7, 3).floor() == -3);
    assert(Rational<long long>(-7, 3).ceil() == -2);
    assert(Rational<long long>(6, 3).is_integer());
    assert(Rational<long long>(1, 3) < Rational<long long>(1, 2));
    assert(sign(Rational<long long>(-1, 5)) == -1);
    assert(abs(Rational<long long>(-1, 5)) == Rational<long long>(1, 5));
    assert(std::abs(a.to_long_double() - 0.75L) < 1e-18L);

    using i128 = __int128_t;
    Rational<i128> wide((i128)1 << 100, 3);
    assert(wide.numerator == (i128)1 << 100);
    assert(wide.denominator == 3);
    assert(wide + Rational<long long>(2, 3)
           == Rational<i128>(((i128)1 << 100) + 2, 3));
    assert(Rational<long long>(1, 2) < Rational<i128>(2, 3));

    cout << "Hello World\n";
    return 0;
}
