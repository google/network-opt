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

#include "network_opt_local.h"
#include "stdlib.h"

int main(int argc, char *argv[]) {
  std::cout << " Command:";
  for (int i = 0; i < argc; ++i) std::cout << " " << argv[i];
  std::cout << std::endl;
  std::string solver = argv[1];
  unsigned int b = atoi(argv[2]), t = atoi(argv[3]);
  network_opt::Problem problem = network_opt::Problem::from_argv(argv);
  network_opt::Params params(b, t);
  if (solver == "opt") {
    network_opt::Solver solver(params);
    network_opt::Node* network = solver.solve(problem);
    network_opt::print_summary(std::cout, problem, network, "");
  } else if (solver == "local") {
    srand(2022);
    network_opt::LocalSolver solver(params);
    network_opt::Node* network = solver.solve(problem);
    network_opt::print_summary(std::cout, problem, network, "");
  }
}
