#pragma once
#include <bits/stdc++.h>

template<class UInt = unsigned long long, int BITS = std::numeric_limits<UInt>::digits>
struct XorBasis {
    std::array<UInt, BITS> basis{};
    int rank = 0;

    bool insert(UInt x) {
        for (int b = BITS - 1; b >= 0; --b) if ((x >> b) & 1) {
            if (!basis[b]) { basis[b] = x; ++rank; return true; }
            x ^= basis[b];
        }
        return false;
    }
    bool contains(UInt x) const {
        for (int b = BITS - 1; b >= 0; --b)
            if ((x >> b) & 1) x ^= basis[b];
        return x == 0;
    }
    UInt max_xor(UInt seed = 0) const {
        UInt answer = seed;
        for (int b = BITS - 1; b >= 0; --b) answer = std::max(answer, answer ^ basis[b]);
        return answer;
    }
};
