#include <bits/stdc++.h>
using namespace std;

class Random {
private:
    uint32_t x2_;
    uint32_t x3_;
    uint64_t c_x1_;

    static uint64_t splitmix64(uint64_t& x) {
        x += 0x9e3779b97f4a7c15ULL;
        uint64_t z = x;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }

public:
    // seedを指定して初期化
    Random(const uint64_t seed = 0) { setSeed(seed); }

    inline void setSeed(const uint64_t seed = 0) {
        if (seed == 0) {
            x2_ = 12345U;
            x3_ = 0xcafef00dU;
            c_x1_ = (0xd15ea5e5ULL << 32) | 23456ULL;
            return;
        }
        uint64_t x = seed;
        x2_ = static_cast<uint32_t>(splitmix64(x));
        x3_ = static_cast<uint32_t>(splitmix64(x));
        c_x1_ = splitmix64(x);
    }

    inline uint32_t next() {
        const uint64_t x = static_cast<uint64_t>(x3_) * 3487286589ULL;
        const uint32_t ret =
            (x3_ ^ x2_) +
            (static_cast<uint32_t>(c_x1_) ^ static_cast<uint32_t>(x >> 32));
        x3_ = x2_;
        x2_ = static_cast<uint32_t>(c_x1_);
        c_x1_ = x + (c_x1_ >> 32);
        return ret;
    }

    inline uint64_t next64() {
        return (static_cast<uint64_t>(next()) << 32) | next();
    }

    inline double nextf() {
        const uint64_t bits = 0x3ff0000000000000ULL |
                              (static_cast<uint64_t>(next()) << 20);
        double ret;
        memcpy(&ret, &bits, sizeof(ret));
        return ret - 1.0;
    }

    inline size_t get(const size_t n) {
        assert(0 < n && n <= static_cast<size_t>(UINT32_MAX));
        return (static_cast<uint64_t>(next()) * n) >> 32;
    }

    inline size_t range(const size_t a, const size_t b) {
        assert(a < b);
        return get(b - a) + a;
    }

    inline size_t range_skip(const size_t a, const size_t b, const size_t skip) {
        assert(a <= skip && skip < b);
        const size_t n = range(a, b - 1);
        return n + (skip <= n);
    }

    inline int64_t rangei(const int64_t a, const int64_t b) {
        assert(a < b);
        return static_cast<int64_t>(get(static_cast<size_t>(b - a))) + a;
    }

    template <typename T>
    inline void shuffle(vector<T>& a) {
        for (size_t i = a.size(); i > 1; --i) {
            swap(a[i - 1], a[get(i)]);
        }
    }

    // 0以上m未満の整数の範囲の乱数
    inline int nextInt(const int m) {
        assert(0 < m);
        return static_cast<int>(get(static_cast<size_t>(m)));
    }

    // 0以上1.0未満の実数の範囲の乱数
    inline double nextDouble() { return nextf(); }

    // 0以上1.0未満の実数の範囲の乱数のlog。焼きなまし法で使いやすい。
    inline double nextLog() { return log(nextDouble()); }

    inline uint32_t nextUInt32() { return next(); }
};
