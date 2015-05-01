#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "vector2D.h"
#include "hmm.h"

int main() {
  vector<double> pi {0.5, 0.3, 0.2};
  vector2D<double> transition(3, 3, 0.0);
  transition(0, 0) = 0.1;
  transition(0, 1) = 0.6;
  transition(0, 2) = 0.3;
  transition(1, 0) = 0.2;
  transition(1, 1) = 0.35;
  transition(1, 2) = 0.45;
  transition(2, 0) = 0.25;
  transition(2, 1) = 0.5;
  transition(2, 2) = 0.25;
  vector2D<double> emission(3, 3, 0.0);
  emission(0, 0) = 0.4;
  emission(1, 0) = 0.6;
  emission(2, 0) = 0;
  emission(0, 1) = 0.8;
  emission(1, 1) = 0.1;
  emission(2, 1) = 0.1;
  emission(0, 2) = 0;
  emission(1, 2) = 0.75;
  emission(2, 2) = 0.25;
  HMM hmm(pi, transition, emission, vector<vector<int>>(1, vector<int>{0, 1, 2, 0}));
  hmm.update();
  auto res = hmm.decode(vector<int>{0, 1, 2, 0});
  while (!res.empty()) {
    cout << res.top() << endl;
    res.pop();
    }
  return 0;
}
