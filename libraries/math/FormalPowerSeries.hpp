#pragma once
#include <bits/stdc++.h>
#include <atcoder/convolution>

template<class Mint, bool UseNTT = true>
struct FormalPowerSeries {
    using FPS = FormalPowerSeries<Mint, UseNTT>;
    std::vector<Mint> a;

    FormalPowerSeries() = default;
    explicit FormalPowerSeries(int n) : a(n) {}
    FormalPowerSeries(const std::vector<Mint>& v) : a(v) {}
    FormalPowerSeries(std::initializer_list<Mint> init) : a(init) {}

    int size() const { return (int)a.size(); }
    bool empty() const { return a.empty(); }
    void resize(int n) { a.resize(n); }
    void shrink() { while (!a.empty() && a.back() == Mint()) a.pop_back(); }
    std::vector<Mint>& vec() { return a; }
    const std::vector<Mint>& vec() const { return a; }

    Mint operator[](int i) const { return 0 <= i && i < size() ? a[i] : Mint(); }
    Mint& operator[](int i) { return a[i]; }

    FPS pre(int n) const {
        n = std::max(n, 0);
        return FPS(std::vector<Mint>(a.begin(), a.begin() + std::min(n, size())));
    }
    FPS rev() const {
        FPS result = *this;
        std::reverse(result.a.begin(), result.a.end());
        return result;
    }

    static std::vector<Mint> multiply_vector(const std::vector<Mint>& lhs, const std::vector<Mint>& rhs) {
        if (lhs.empty() || rhs.empty()) return {};
        if constexpr (UseNTT) {
            if (std::min(lhs.size(), rhs.size()) <= 60) {
                std::vector<Mint> result(lhs.size() + rhs.size() - 1);
                for (int i = 0; i < (int)lhs.size(); ++i) for (int j = 0; j < (int)rhs.size(); ++j) result[i + j] += lhs[i] * rhs[j];
                return result;
            }
            return atcoder::convolution(lhs, rhs);
        } else {
            std::vector<Mint> result(lhs.size() + rhs.size() - 1);
            for (int i = 0; i < (int)lhs.size(); ++i) for (int j = 0; j < (int)rhs.size(); ++j) result[i + j] += lhs[i] * rhs[j];
            return result;
        }
    }

    FPS& operator+=(const FPS& rhs) {
        if (size() < rhs.size()) resize(rhs.size());
        for (int i = 0; i < rhs.size(); ++i) a[i] += rhs.a[i];
        return *this;
    }
    FPS& operator-=(const FPS& rhs) {
        if (size() < rhs.size()) resize(rhs.size());
        for (int i = 0; i < rhs.size(); ++i) a[i] -= rhs.a[i];
        return *this;
    }
    FPS& operator*=(const FPS& rhs) {
        a = multiply_vector(a, rhs.a);
        return *this;
    }
    FPS& operator+=(Mint c) {
        if (empty()) resize(1);
        a[0] += c;
        return *this;
    }
    FPS& operator-=(Mint c) {
        if (empty()) resize(1);
        a[0] -= c;
        return *this;
    }
    FPS& operator*=(Mint c) {
        for (Mint& x : a) x *= c;
        return *this;
    }
    FPS& operator/=(Mint c) {
        Mint inv_c = Mint(1) / c;
        for (Mint& x : a) x *= inv_c;
        return *this;
    }

    FPS operator+() const { return *this; }
    FPS operator-() const {
        FPS result = *this;
        for (Mint& x : result.a) x = -x;
        return result;
    }
    friend FPS operator+(FPS lhs, const FPS& rhs) { return lhs += rhs; }
    friend FPS operator-(FPS lhs, const FPS& rhs) { return lhs -= rhs; }
    friend FPS operator*(FPS lhs, const FPS& rhs) { return lhs *= rhs; }
    friend FPS operator+(FPS lhs, Mint rhs) { return lhs += rhs; }
    friend FPS operator-(FPS lhs, Mint rhs) { return lhs -= rhs; }
    friend FPS operator*(FPS lhs, Mint rhs) { return lhs *= rhs; }
    friend FPS operator/(FPS lhs, Mint rhs) { return lhs /= rhs; }
    friend FPS operator+(Mint lhs, FPS rhs) { return rhs += lhs; }
    friend FPS operator-(Mint lhs, FPS rhs) { return -(rhs -= lhs); }
    friend FPS operator*(Mint lhs, FPS rhs) { return rhs *= lhs; }
    bool operator==(const FPS& rhs) const { return a == rhs.a; }
    bool operator!=(const FPS& rhs) const { return !(*this == rhs); }

    FPS diff() const {
        if (size() <= 1) return FPS();
        FPS result(size() - 1);
        for (int i = 1; i < size(); ++i) result.a[i - 1] = a[i] * Mint(i);
        return result;
    }
    FPS integral() const {
        FPS result(size() + 1);
        for (int i = 0; i < size(); ++i) result.a[i + 1] = a[i] / Mint(i + 1);
        return result;
    }
    FPS inv(int n) const {
        assert(n >= 0);
        if (n == 0) return FPS();
        assert(!empty() && a[0] != Mint());
        FPS result{Mint(1) / a[0]};
        for (int m = 1; m < n; m <<= 1) {
            int next = std::min(2 * m, n);
            FPS f = pre(next);
            FPS g = result * result * f;
            result *= Mint(2);
            result -= g.pre(next);
            result.resize(next);
        }
        return result.pre(n);
    }
    FPS log(int n) const {
        assert(n >= 0);
        if (n == 0) return FPS();
        assert(!empty() && a[0] == Mint(1));
        return (diff() * inv(n)).pre(n - 1).integral().pre(n);
    }
    FPS exp(int n) const {
        assert(n >= 0);
        if (n == 0) return FPS();
        assert(empty() || a[0] == Mint());
        FPS result{Mint(1)};
        for (int m = 1; m < n; m <<= 1) {
            int next = std::min(2 * m, n);
            FPS correction = pre(next) - result.log(next);
            correction += Mint(1);
            result = (result * correction).pre(next);
        }
        return result.pre(n);
    }
    FPS pow(long long k, int n) const {
        assert(k >= 0 && n >= 0);
        if (n == 0) return FPS();
        if (k == 0) {
            FPS result(n);
            result.a[0] = Mint(1);
            return result;
        }
        int shift = 0;
        while (shift < size() && a[shift] == Mint()) ++shift;
        if (shift == size() || (__int128)shift * k >= n) return FPS(n);
        Mint c = a[shift];
        FPS g(std::vector<Mint>(a.begin() + shift, a.end()));
        g /= c;
        FPS result = (g.log(n - shift * k) * Mint(k)).exp(n - shift * k) * c.pow(k);
        FPS shifted(n);
        for (int i = 0; i < result.size() && i + shift * k < n; ++i) shifted.a[i + shift * k] = result.a[i];
        return shifted;
    }
    Mint eval(Mint x) const {
        Mint result = 0;
        for (int i = size() - 1; i >= 0; --i) result = result * x + a[i];
        return result;
    }

    std::pair<FPS, FPS> divmod(FPS rhs) const {
        rhs.shrink();
        assert(!rhs.empty());
        FPS lhs = *this;
        lhs.shrink();
        if (lhs.size() < rhs.size()) return {FPS(), lhs};
        int q_size = lhs.size() - rhs.size() + 1;
        FPS q = (lhs.rev().pre(q_size) * rhs.rev().inv(q_size)).pre(q_size).rev();
        FPS r = lhs - rhs * q;
        r.resize(rhs.size() - 1);
        r.shrink();
        return {q, r};
    }
    friend FPS operator/(const FPS& lhs, const FPS& rhs) { return lhs.divmod(rhs).first; }
    friend FPS operator%(const FPS& lhs, const FPS& rhs) { return lhs.divmod(rhs).second; }
};
