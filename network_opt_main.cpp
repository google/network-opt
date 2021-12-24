/*
Copyright 2022 Google LLC
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    https://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "network_opt.h"

int main(int argc, char *argv[]) {
  std::cout << " Command:";
  for (int i = 0; i < argc; ++i) std::cout << " " << argv[i];
  std::cout << std::endl;
  unsigned int n = atoi(argv[1]), t = atoi(argv[2]), b = atoi(argv[3]);
  std::string series = argv[4];
  SERIES = E12_SERIES;
  if (series == "INT") SERIES = INT_SERIES;
  if (series == "ONE") SERIES = ONE_SERIES;
  Bounder* bounder = b ? new Bounder() : NULL;
  Tabulator* tabulator = t ? new Tabulator(t) : NULL;
  Solver solver(bounder, tabulator);
  Node* network = solver.solve(n);
  print_summary(cout, network, n, "");
  delete tabulator;
  delete bounder;
}
