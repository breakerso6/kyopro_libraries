#pragma once
#include <bits/stdc++.h>

// Returns the leftmost minimum column of every row of a totally monotone matrix.
template<class F>
std::vector<int> smawk(int rows, int columns, F value) {
    std::vector<int> answer(rows, -1), all_rows(rows), all_columns(columns);
    std::iota(all_rows.begin(), all_rows.end(), 0);
    std::iota(all_columns.begin(), all_columns.end(), 0);
    std::function<void(const std::vector<int>&, const std::vector<int>&)> solve =
        [&](const std::vector<int>& rs, const std::vector<int>& cs) {
            if (rs.empty()) return;
            std::vector<int> reduced;
            for (int column : cs) {
                while (!reduced.empty()) {
                    int row = rs[reduced.size() - 1];
                    if (!(value(row, column) < value(row, reduced.back()))) break;
                    reduced.pop_back();
                }
                if (reduced.size() < rs.size()) reduced.push_back(column);
            }
            std::vector<int> odd_rows;
            for (int i = 1; i < (int)rs.size(); i += 2) odd_rows.push_back(rs[i]);
            solve(odd_rows, reduced);
            for (int i = 0; i < (int)rs.size(); i += 2) {
                int begin = 0;
                if (i > 0) begin = std::find(reduced.begin(), reduced.end(), answer[rs[i - 1]]) - reduced.begin();
                int end = reduced.size() - 1;
                if (i + 1 < (int)rs.size())
                    end = std::find(reduced.begin(), reduced.end(), answer[rs[i + 1]]) - reduced.begin();
                int best = begin;
                for (int j = begin + 1; j <= end; ++j)
                    if (value(rs[i], reduced[j]) < value(rs[i], reduced[best])) best = j;
                answer[rs[i]] = reduced[best];
            }
        };
    if (columns > 0) solve(all_rows, all_columns);
    return answer;
}
