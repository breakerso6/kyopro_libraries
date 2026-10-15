// GitHub: https://github.com/breakerso6/kyopro_libraries/blob/feature/icpc-extra-libraries/libraries/algorithm/SMAWK.hpp
// SPDX-License-Identifier: CC0-1.0

#pragma once
#include <bits/stdc++.h>

// Returns the leftmost minimum column of every row of a totally monotone matrix.
template<class F>
std::vector<int> smawk(int rows, int columns, F value) {
    std::vector<int> answer(rows, -1), all_rows(rows), all_columns(columns);
    std::iota(all_rows.begin(), all_rows.end(), 0);
    std::iota(all_columns.begin(), all_columns.end(), 0);
    auto solve = [&](auto&& self, const std::vector<int>& rs, const std::vector<int>& cs) -> void {
        if (rs.empty()) return;
        std::vector<int> reduced;
        reduced.reserve(std::min(rs.size(), cs.size()));
        for (int column : cs) {
            while (!reduced.empty()) {
                int row = rs[reduced.size() - 1];
                if (!(value(row, column) < value(row, reduced.back()))) break;
                reduced.pop_back();
            }
            if (reduced.size() < rs.size()) reduced.push_back(column);
        }
        std::vector<int> odd_rows;
        odd_rows.reserve(rs.size() / 2);
        for (int i = 1; i < (int)rs.size(); i += 2) odd_rows.push_back(rs[i]);
        self(self, odd_rows, reduced);
        // Neighboring odd-row minima are monotone. Advance their column
        // cursor once instead of searching from column zero for each row.
        int begin = 0;
        for (int i = 0; i < (int)rs.size(); i += 2) {
            int end = reduced.size() - 1;
            if (i + 1 < (int)rs.size()) {
                end = begin;
                while (reduced[end] != answer[rs[i + 1]]) ++end;
            }
            int best = begin;
            for (int j = begin + 1; j <= end; ++j)
                if (value(rs[i], reduced[j]) < value(rs[i], reduced[best])) best = j;
            answer[rs[i]] = reduced[best];
            begin = end;
        }
    };
    if (columns > 0) solve(solve, all_rows, all_columns);
    return answer;
}
