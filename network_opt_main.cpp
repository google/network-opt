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
  unsigned int b = atoi(argv[1]), t = atoi(argv[2]), n = atoi(argv[3]);
  std::string s = argv[4], g = argv[5];
  Ratio* series = network_opt::E12_SERIES;
  Ratio target = Ratio(n);
  bool square = true;
  if (s ==   "INT") series = network_opt::INT_SERIES;
  if (s ==   "ONE") series = network_opt::ONE_SERIES;
  if (g ==     "E") { square = false; target = network_opt::RATIO_E; }
  if (g ==    "PI") { square = false; target = network_opt::RATIO_PI; }
  if (g ==   "PHI") { square = false; target = network_opt::RATIO_PHI; }
  if (g == "SQRT2") { square = false; target = network_opt::RATIO_SQRT2; }
  network_opt::Problem problem(series, n, target, square);
  network_opt::Bounder* bounder = b ? new network_opt::Bounder() : NULL;
  network_opt::Tabulator* tabulator = t ? new network_opt::Tabulator(t) : NULL;
  network_opt::Solver solver(bounder, tabulator);
  network_opt::Node* network = solver.solve(problem);
  network_opt::print_summary(std::cout, problem, network, "");
  delete tabulator;
  delete bounder;
}
