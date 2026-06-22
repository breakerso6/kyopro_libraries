#include <bits/stdc++.h>
using namespace std;

#include "libraries/data_structure/Indexset.hpp"

static void assert_state(IndexSet& s, const vector<bool>& in) {
  int expected_size = 0;
  for (bool x : in) expected_size += x;
  assert(s.size() == expected_size);
  for (int i = 0; i < (int)in.size(); i++) {
    assert(s.contain(i) == in[i]);
  }
}

int main() {
  {
    IndexSet s(5);
    vector<bool> in(5, true);
    assert_state(s, in);

    s.erase(2);
    in[2] = false;
    assert_state(s, in);

    s.erase(2);
    assert_state(s, in);

    s.push(2);
    in[2] = true;
    assert_state(s, in);

    s.push(2);
    assert_state(s, in);

    s.clear();
    fill(in.begin(), in.end(), false);
    assert_state(s, in);
    assert(s.random() == -1);

    s.push(4);
    in[4] = true;
    assert_state(s, in);
    for (int t = 0; t < 100; t++) {
      assert(s.random() == 4);
    }

    s.fill();
    fill(in.begin(), in.end(), true);
    assert_state(s, in);
  }

  {
    const int n = 30;
    IndexSet s(n);
    vector<bool> in(n, true);

    for (int t = 0; t < 10000; t++) {
      int op = rnd.nextInt(5);
      int x = rnd.nextInt(n);
      if (op == 0) {
        s.push(x);
        in[x] = true;
      } else if (op == 1) {
        s.erase(x);
        in[x] = false;
      } else if (op == 2) {
        s.clear();
        fill(in.begin(), in.end(), false);
      } else if (op == 3) {
        s.fill();
        fill(in.begin(), in.end(), true);
      } else {
        int y = s.random();
        if (s.size() == 0) {
          assert(y == -1);
        } else {
          assert(0 <= y && y < n);
          assert(in[y]);
        }
      }
      assert_state(s, in);
    }
  }
}
