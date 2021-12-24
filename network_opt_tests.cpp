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

#include "network_opt_utils.h"

void test_node() {
  network_opt::Node* network = NULL;
  
  network = &N()[NT(1)][N()[NT(2)][NT(3)][NT(4)]][NT(5)];
  assert(network->to_string() == "1+(2|3|4)+5");
  assert(network->to_string(true) == "1$+$(2|3|4)$+$5");
  assert(network->to_network() == "N()[N(0)][N()[N(1)][N(2)][N(3)]][N(4)]");
  delete network;

  network = &N()[N()[N()[NT(3)][NT(7)]][N()[NT(1)][NT(2)][NT(5)]][N()[NT(4)][NT(6)]]];
  assert(network->to_string() == "(3+7)|(1+2+5)|(4+6)");
  delete network;

  network = &N()[NT({1,3})][NT(7)[NT(6)[NT({2,5})]][NT(4)]];
  assert(network->to_network() == "N()[N({0,2})][N(6)[N(5)[N({1,4})]][N(3)]]");
  network->leafify();
  assert(network->to_network() == "N()[N()[N(0)][N(2)]][N()[N()[N()[N(1)][N(4)]][N(5)]][N(3)][N(6)]]");
  delete network;
}

void test_network_evaluator() {
  network_opt::Node* network = NULL;

  network = &N()[NT(1)][NT(2)][NT(3)][NT(4)][NT(5)];
  assert(network_evaluator.evaluate_cost(network, 5) == Ratio(220, 1));
  delete network;

  network = &N()[N()[NT(1)][NT(2)][NT(3)][NT(4)][NT(5)]];
  assert(network_evaluator.evaluate_cost(network, 5) == Ratio(90245, 18769));
  delete network;

  network = &N()[NT(1)][N()[NT(2)][N()[NT(3)][N()[NT(4)][NT(5)]]]];
  assert(network_evaluator.evaluate_cost(network, 5) == Ratio(4156, 4225));
  delete network;

  network = &N()[NT(1)][NT({2,3,4})[NT({5,6,7})]][NT(8)];
  assert(network_evaluator.evaluate_cost(network, 8,  1) == Ratio(217, 1));
  assert(network_evaluator.evaluate_cost(network, 8, -1) == Ratio(4211777, 49729));
  delete network;
}

void test_expander() {
  network_opt::Node* network = NULL;

  network = &NT({1,2,3});
  assert(network_opt::Expander(network).expandable() == network);
  delete network;

  network = &N()[NT(1)][NT({2,3,4})][NT({5,6})];
  assert(network_opt::Expander(network).expandable()->values == Values({1,2,3})); // means {2,3,4}
  delete network;

  network = &N()[NT(1)][NT({2,3})][NT({5,6,7})];
  assert(network_opt::Expander(network).expandable()->values == Values({4,5,6})); // means {5,6,7}
  delete network;

  network = &N()[NT(1)][NT(5)[NT({2,3,4})]][NT({6,7})];
  assert(network_opt::Expander(network).expandable()->values == Values({1,2,3})); // means {2,3,4}
  delete network;

  network = &N()[NT(1)][NT(2)][NT(3)];
  assert(network_opt::Expander(network).expandable() == NULL);
  delete network;

  // Verify that we can see more than one expandable
  network = &N()[NT(1)][NT({2,3,4})[NT({5,6,7})]];
  network_opt::Expander expander(network);
  assert(expander.expandable()->values == Values({1,2,3})); // means {2,3,4}
  assert(expander.expandable()->values == Values({4,5,6})); // means {5,6,7}
  assert(expander.expandable() == NULL);
  delete network;
  
  // Verify that we can modify the contents of the given list
  network = &N()[NT({1,2,3})][NT(4)][NT(5)];
  network_opt::Node* expandable = network_opt::Expander(network).expandable();
  expandable->values.push_back(5);
  assert(network->children.front()->values == Values({0,1,2,5})); // means {1,2,3,6}
  delete network;
}

void test_tabulator() {
  network_opt::Tabulator tabulator(3);
  tabulator.tabulate(7);
  network_opt::Node* network = NULL;

  network = &N()[N()[NT(1)][NT(3)]][N()[NT({2,5,6})][NT(4)][NT(7)]];
  network_opt::Expander expander_a(network);
  network_opt::Node* expandable_a = expander_a.expandable();
  assert(expandable_a->values == Values({1,4,5})); // means {2,5,6}
  Values values_a = expandable_a->values; expandable_a->values.clear();
  network_opt::Node* replacement_a = tabulator.binary_search(network, expandable_a, values_a, 7);
  assert(replacement_a->ratio == Ratio(52, 7));
  delete network;

  network = &N()[NT({1,3,4})][NT(7)][NT({2,5,6})];
  network_opt::Expander expander_b(network);
  network_opt::Node* expandable_b0 = expander_b.expandable();
  network_opt::Node* expandable_b1 = expander_b.expandable();
  assert(expandable_b0->values == Values({1,4,5})); // means {2,5,6}
  assert(expandable_b1->values == Values({0,2,3})); // means {1,3,4}
  Values values_b0 = expandable_b0->values; expandable_b0->values.clear();
  Values values_b1 = expandable_b1->values; expandable_b1->values.clear();
  std::pair<network_opt::Node*,network_opt::Node*> replacement_b = tabulator.linear_search(
      network, expandable_b0, expandable_b1, values_b0, values_b1, 7);
  assert(replacement_b.first->ratio  == Ratio(15, 13));
  assert(replacement_b.second->ratio == Ratio(12, 19));
  delete network;
}

void check_network(network_opt::Node* network, Ratio ratio) {
  assert(network->ratio == ratio);
}

void test_solver(network_opt::Bounder* bounder = NULL, network_opt::Tabulator* tabulator = NULL) {
  network_opt::Solver solver(bounder, tabulator);
  check_network(solver.solve(2), Ratio(14, 9));
  check_network(solver.solve(3), Ratio(3, 4));
  check_network(solver.solve(4), Ratio(0, 1));
  check_network(solver.solve(5), Ratio(5, 81));
  check_network(solver.solve(6), Ratio(278, 178929));
  check_network(solver.solve(7), Ratio(1, 2304));
}

int main() {
  test_node();
  test_network_evaluator();
  test_expander();
  test_tabulator();
  test_solver();

  network_opt::Bounder bounder;
  test_solver(&bounder);

  network_opt::Tabulator tabulator(3);
  test_solver(&bounder, &tabulator);

  std::cout << "*** TESTS PASS ***" << std::endl;
}
