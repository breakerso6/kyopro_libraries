#pragma once
#include <bits/stdc++.h>

template<class T>
using Matrix = std::vector<std::vector<T>>;

template<class Int = long long>
struct Rational {
    Int numerator, denominator;
    Rational(Int numerator_ = 0, Int denominator_ = 1) : numerator(numerator_), denominator(denominator_) {
        assert(denominator != 0);
        normalize();
    }
    void normalize() {
        if (denominator < 0) numerator = -numerator, denominator = -denominator;
        Int g = std::gcd(numerator < 0 ? -numerator : numerator, denominator);
        if (g) numerator /= g, denominator /= g;
    }
    Rational operator+(Rational r) const { return {numerator * r.denominator + r.numerator * denominator, denominator * r.denominator}; }
    Rational operator-(Rational r) const { return {numerator * r.denominator - r.numerator * denominator, denominator * r.denominator}; }
    Rational operator-() const { return {-numerator, denominator}; }
    Rational operator*(Rational r) const { return {numerator * r.numerator, denominator * r.denominator}; }
    Rational operator/(Rational r) const { assert(r.numerator != 0); return {numerator * r.denominator, denominator * r.numerator}; }
    Rational& operator+=(Rational r) { return *this = *this + r; }
    Rational& operator-=(Rational r) { return *this = *this - r; }
    Rational& operator*=(Rational r) { return *this = *this * r; }
    bool operator==(Rational r) const { return numerator == r.numerator && denominator == r.denominator; }
    bool operator!=(Rational r) const { return !(*this == r); }
};

template<class T>
Matrix<T> matrix_identity(int n) {
    Matrix<T> a(n, std::vector<T>(n));
    for (int i = 0; i < n; ++i) a[i][i] = 1;
    return a;
}

template<class T>
Matrix<T> matrix_mul(const Matrix<T>& a, const Matrix<T>& b) {
    int n = (int)a.size(), m = (int)b[0].size(), p = (int)b.size();
    assert(!a.empty() && !b.empty() && (int)a[0].size() == p);
    Matrix<T> c(n, std::vector<T>(m));
    for (int i = 0; i < n; ++i) for (int k = 0; k < p; ++k) if (a[i][k] != T())
        for (int j = 0; j < m; ++j) c[i][j] += a[i][k] * b[k][j];
    return c;
}

template<class T>
Matrix<T> matrix_pow(Matrix<T> a, long long e) {
    assert(!a.empty() && a.size() == a[0].size() && e >= 0);
    Matrix<T> result = matrix_identity<T>((int)a.size());
    while (e) {
        if (e & 1) result = matrix_mul(result, a);
        a = matrix_mul(a, a);
        e >>= 1;
    }
    return result;
}

template<class T>
T determinant(Matrix<T> a) {
    int n = (int)a.size();
    assert(n == 0 || a[0].size() == a.size());
    T det = 1;
    for (int i = 0; i < n; ++i) {
        int pivot = i;
        while (pivot < n && a[pivot][i] == T()) ++pivot;
        if (pivot == n) return 0;
        if (pivot != i) std::swap(a[pivot], a[i]), det = -det;
        det *= a[i][i];
        T inv = T(1) / a[i][i];
        for (int r = i + 1; r < n; ++r) {
            T factor = a[r][i] * inv;
            for (int c = i; c < n; ++c) a[r][c] -= factor * a[i][c];
        }
    }
    return det;
}

template<class T>
std::optional<Matrix<T>> inverse_matrix(Matrix<T> a) {
    int n = (int)a.size();
    assert(n == 0 || a[0].size() == a.size());
    Matrix<T> b = matrix_identity<T>(n);
    for (int i = 0; i < n; ++i) {
        int pivot = i;
        while (pivot < n && a[pivot][i] == T()) ++pivot;
        if (pivot == n) return std::nullopt;
        std::swap(a[pivot], a[i]); std::swap(b[pivot], b[i]);
        T inv = T(1) / a[i][i];
        for (int c = 0; c < n; ++c) a[i][c] *= inv, b[i][c] *= inv;
        for (int r = 0; r < n; ++r) if (r != i && a[r][i] != T()) {
            T factor = a[r][i];
            for (int c = 0; c < n; ++c) a[r][c] -= factor * a[i][c], b[r][c] -= factor * b[i][c];
        }
    }
    return b;
}

template<class Int>
std::optional<Matrix<Rational<Int>>> inverse_matrix_rational(const Matrix<Int>& a) {
    int n = (int)a.size();
    Matrix<Rational<Int>> b(n, std::vector<Rational<Int>>(n));
    for (int i = 0; i < n; ++i) {
        assert((int)a[i].size() == n);
        for (int j = 0; j < n; ++j) b[i][j] = Rational<Int>(a[i][j]);
    }
    return inverse_matrix(b);
}

template<class Int>
Int determinant_bareiss(Matrix<Int> a) {
    int n = (int)a.size();
    assert(n == 0 || a[0].size() == a.size());
    Int sign = 1, prev = 1;
    for (int k = 0; k < n - 1; ++k) {
        int pivot = k;
        while (pivot < n && a[pivot][k] == 0) ++pivot;
        if (pivot == n) return 0;
        if (pivot != k) std::swap(a[pivot], a[k]), sign = -sign;
        for (int i = k + 1; i < n; ++i) for (int j = k + 1; j < n; ++j)
            a[i][j] = (a[i][j] * a[k][k] - a[i][k] * a[k][j]) / prev;
        prev = a[k][k];
    }
    return n == 0 ? 1 : sign * a[n - 1][n - 1];
}
