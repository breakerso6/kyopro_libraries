#pragma once
#include <bits/stdc++.h>

namespace rational_detail {
template<class Int>
Int abs_value(Int value) {
    return value < 0 ? -value : value;
}

template<class Int>
Int gcd(Int a, Int b) {
    a = abs_value(a);
    b = abs_value(b);
    while (b != 0) {
        Int remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}
}  // namespace rational_detail

template<class Int = long long>
struct Rational {
    Int numerator = 0;
    Int denominator = 1;

    Rational(Int numerator_ = 0, Int denominator_ = 1)
        : numerator(numerator_), denominator(denominator_) {
        normalize();
    }

    template<class OtherInt>
    Rational(const Rational<OtherInt>& other)
        : numerator((Int)other.numerator), denominator((Int)other.denominator) {
        normalize();
    }

    void normalize() {
        assert(denominator != 0);
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        Int divisor = rational_detail::gcd(numerator, denominator);
        numerator /= divisor;
        denominator /= divisor;
    }

    long double to_long_double() const {
        return (long double)numerator / (long double)denominator;
    }
    bool is_integer() const { return denominator == 1; }
    Int floor() const {
        if (numerator >= 0) return numerator / denominator;
        return -((-numerator + denominator - 1) / denominator);
    }
    Int ceil() const {
        if (numerator >= 0) return (numerator + denominator - 1) / denominator;
        return -((-numerator) / denominator);
    }

    Rational operator+() const { return *this; }
    Rational operator-() const { return {-numerator, denominator}; }

    friend Rational operator+(Rational a, Rational b) {
        Int divisor = rational_detail::gcd(a.denominator, b.denominator);
        Int a_scale = b.denominator / divisor;
        Int b_scale = a.denominator / divisor;
        return {a.numerator * a_scale + b.numerator * b_scale,
                a.denominator * a_scale};
    }
    friend Rational operator-(Rational a, Rational b) { return a + (-b); }
    friend Rational operator*(Rational a, Rational b) {
        Int left_divisor = rational_detail::gcd(a.numerator, b.denominator);
        Int right_divisor = rational_detail::gcd(b.numerator, a.denominator);
        return {(a.numerator / left_divisor) * (b.numerator / right_divisor),
                (a.denominator / right_divisor) * (b.denominator / left_divisor)};
    }
    friend Rational operator/(Rational a, Rational b) {
        assert(b.numerator != 0);
        if (b.numerator < 0) {
            b.numerator = -b.numerator;
            b.denominator = -b.denominator;
        }
        Int left_divisor = rational_detail::gcd(a.numerator, b.numerator);
        Int right_divisor = rational_detail::gcd(b.denominator, a.denominator);
        return {(a.numerator / left_divisor) * (b.denominator / right_divisor),
                (a.denominator / right_divisor) * (b.numerator / left_divisor)};
    }

    Rational& operator+=(Rational other) { return *this = *this + other; }
    Rational& operator-=(Rational other) { return *this = *this - other; }
    Rational& operator*=(Rational other) { return *this = *this * other; }
    Rational& operator/=(Rational other) { return *this = *this / other; }

    friend bool operator==(Rational a, Rational b) {
        return a.numerator == b.numerator && a.denominator == b.denominator;
    }
    friend bool operator!=(Rational a, Rational b) { return !(a == b); }
    friend bool operator<(Rational a, Rational b) {
        return a.numerator * b.denominator < b.numerator * a.denominator;
    }
    friend bool operator>(Rational a, Rational b) { return b < a; }
    friend bool operator<=(Rational a, Rational b) { return !(b < a); }
    friend bool operator>=(Rational a, Rational b) { return !(a < b); }
};

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
auto operator+(Rational<LeftInt> a, Rational<RightInt> b) {
    using CommonInt = std::common_type_t<LeftInt, RightInt>;
    return Rational<CommonInt>(a) + Rational<CommonInt>(b);
}

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
auto operator-(Rational<LeftInt> a, Rational<RightInt> b) {
    using CommonInt = std::common_type_t<LeftInt, RightInt>;
    return Rational<CommonInt>(a) - Rational<CommonInt>(b);
}

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
auto operator*(Rational<LeftInt> a, Rational<RightInt> b) {
    using CommonInt = std::common_type_t<LeftInt, RightInt>;
    return Rational<CommonInt>(a) * Rational<CommonInt>(b);
}

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
auto operator/(Rational<LeftInt> a, Rational<RightInt> b) {
    using CommonInt = std::common_type_t<LeftInt, RightInt>;
    return Rational<CommonInt>(a) / Rational<CommonInt>(b);
}

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
bool operator==(Rational<LeftInt> a, Rational<RightInt> b) {
    using CommonInt = std::common_type_t<LeftInt, RightInt>;
    return Rational<CommonInt>(a) == Rational<CommonInt>(b);
}

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
bool operator!=(Rational<LeftInt> a, Rational<RightInt> b) {
    return !(a == b);
}

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
bool operator<(Rational<LeftInt> a, Rational<RightInt> b) {
    using CommonInt = std::common_type_t<LeftInt, RightInt>;
    return Rational<CommonInt>(a) < Rational<CommonInt>(b);
}

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
bool operator>(Rational<LeftInt> a, Rational<RightInt> b) {
    return b < a;
}

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
bool operator<=(Rational<LeftInt> a, Rational<RightInt> b) {
    return !(b < a);
}

template<class LeftInt, class RightInt,
         std::enable_if_t<!std::is_same_v<LeftInt, RightInt>, int> = 0>
bool operator>=(Rational<LeftInt> a, Rational<RightInt> b) {
    return !(a < b);
}

template<class Int>
int sign(Rational<Int> value) {
    return (value.numerator > 0) - (value.numerator < 0);
}

template<class Int>
Rational<Int> abs(Rational<Int> value) {
    return value.numerator < 0 ? -value : value;
}
