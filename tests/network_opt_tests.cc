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

#include "gtest/gtest.h"

#include "../src/network_opt_utils.h"

namespace network_opt {
namespace {

TEST(NodeTest, AllTests) {
  Problem problem5(INT_SERIES, 5, Ratio(5), true);
  Problem problem7(INT_SERIES, 7, Ratio(7), true);
  Node* network = NULL;

  network = &N()[NT(1)][N()[NT(2)][NT(3)][NT(4)]][NT(5)];
  EXPECT_EQ(network->to_string(problem5), "1+(2|3|4)+5");
  EXPECT_EQ(network->to_string(problem5, true), "1$+$(2|3|4)$+$5");
  EXPECT_EQ(network->to_network(), "N()[N(0)][N()[N(1)][N(2)][N(3)]][N(4)]");
  delete network;

  network = &N()[N()[N()[NT(3)][NT(7)]][N()[NT(1)][NT(2)][NT(5)]][N()[NT(4)][NT(6)]]];
  EXPECT_EQ(network->to_string(problem7), "(3+7)|(1+2+5)|(4+6)");
  delete network;

  network = &N()[NT({1,3})][NT(7)[NT(6)[NT({2,5})]][NT(4)]];
  EXPECT_EQ(network->to_network(), "N()[N({0,2})][N(6)[N(5)[N({1,4})]][N(3)]]");
  network->leafify();
  EXPECT_EQ(network->to_network(), "N()[N()[N(0)][N(2)]][N()[N()[N()[N(1)][N(4)]][N(5)]][N(3)][N(6)]]");
  delete network;
}

TEST(NetworkEvaluatorTest, AllTests) {
  Problem problem5(INT_SERIES, 5, Ratio(5), true);
  Problem problem8(INT_SERIES, 8, Ratio(8), true);
  Node* network = NULL;

  network = &N()[NT(1)][NT(2)][NT(3)][NT(4)][NT(5)];
  EXPECT_EQ(network_evaluator.evaluate_cost(problem5, network), Ratio(220, 1));
  delete network;

  network = &N()[N()[NT(1)][NT(2)][NT(3)][NT(4)][NT(5)]];
  EXPECT_EQ(network_evaluator.evaluate_cost(problem5, network), Ratio(90245, 18769));
  delete network;

  network = &N()[NT(1)][N()[NT(2)][N()[NT(3)][N()[NT(4)][NT(5)]]]];
  EXPECT_EQ(network_evaluator.evaluate_cost(problem5, network), Ratio(4156, 4225));
  delete network;

  network = &N()[NT(1)][NT({2,3,4})[NT({5,6,7})]][NT(8)];
  EXPECT_EQ(network_evaluator.evaluate_cost(problem8, network,  1), Ratio(217, 1));
  EXPECT_EQ(network_evaluator.evaluate_cost(problem8, network, -1), Ratio(4211777, 49729));
  delete network;
}

TEST(ExpanderTest, AllTests) {
  Node* network = NULL;

  network = &NT({1,2,3});
  EXPECT_EQ(Expander(network).expandable(), network);
  delete network;

  network = &N()[NT(1)][NT({2,3,4})][NT({5,6})];
  EXPECT_EQ(Expander(network).expandable()->values, Values({1,2,3})); // means {2,3,4}
  delete network;

  network = &N()[NT(1)][NT({2,3})][NT({5,6,7})];
  EXPECT_EQ(Expander(network).expandable()->values, Values({4,5,6})); // means {5,6,7}
  delete network;

  network = &N()[NT(1)][NT(5)[NT({2,3,4})]][NT({6,7})];
  EXPECT_EQ(Expander(network).expandable()->values, Values({1,2,3})); // means {2,3,4}
  delete network;

  network = &N()[NT(1)][NT(2)][NT(3)];
  EXPECT_EQ(Expander(network).expandable(), nullptr);
  delete network;

  // Verify that we can see more than one expandable
  network = &N()[NT(1)][NT({2,3,4})[NT({5,6,7})]];
  Expander expander(network);
  EXPECT_EQ(expander.expandable()->values, Values({1,2,3})); // means {2,3,4}
  EXPECT_EQ(expander.expandable()->values, Values({4,5,6})); // means {5,6,7}
  EXPECT_EQ(expander.expandable(), nullptr);
  delete network;

  // Verify that we can modify the contents of the given list
  network = &N()[NT({1,2,3})][NT(4)][NT(5)];
  Node* expandable = Expander(network).expandable();
  expandable->values.push_back(5);
  EXPECT_EQ(network->children.front()->values, Values({0,1,2,5})); // means {1,2,3,6}
  delete network;
}

TEST(TabulatorTest, AllTests) {
  Problem problem7(INT_SERIES, 7, Ratio(7), true);
  Tabulator tabulator(3);
  tabulator.tabulate(problem7);
  Node* network = NULL;

  network = &N()[N()[NT(1)][NT(3)]][N()[NT({2,5,6})][NT(4)][NT(7)]];
  Expander expander_a(network);
  Node* expandable_a = expander_a.expandable();
  EXPECT_EQ(expandable_a->values, Values({1,4,5})); // means {2,5,6}
  Values values_a = expandable_a->values; expandable_a->values.clear();
  Node* replacement_a = tabulator.binary_search(problem7, network, expandable_a, values_a);
  EXPECT_EQ(replacement_a->ratio, Ratio(52, 7));
  delete network;

  network = &N()[NT({1,3,4})][NT(7)][NT({2,5,6})];
  Expander expander_b(network);
  Node* expandable_b0 = expander_b.expandable();
  Node* expandable_b1 = expander_b.expandable();
  EXPECT_EQ(expandable_b0->values, Values({1,4,5})); // means {2,5,6}
  EXPECT_EQ(expandable_b1->values, Values({0,2,3})); // means {1,3,4}
  Values values_b0 = expandable_b0->values; expandable_b0->values.clear();
  Values values_b1 = expandable_b1->values; expandable_b1->values.clear();
  std::pair<Node*,Node*> replacement_b = tabulator.linear_search(
      problem7, network, expandable_b0, expandable_b1, values_b0, values_b1);
  EXPECT_EQ(replacement_b.first->ratio, Ratio(15, 13));
  EXPECT_EQ(replacement_b.second->ratio, Ratio(12, 19));
  delete network;
}

void check_network(Node* network, Ratio ratio) {
  EXPECT_EQ(network->ratio, ratio);
}

void test_solver(bool b = false, unsigned int t = true) {
  Params params(b, t);
  Solver solver(params);
  check_network(solver.solve(Problem(INT_SERIES, 2, Ratio(2), true)), Ratio(14, 9));
  check_network(solver.solve(Problem(INT_SERIES, 3, Ratio(3), true)), Ratio(3, 4));
  check_network(solver.solve(Problem(INT_SERIES, 4, Ratio(4), true)), Ratio(0, 1));
  check_network(solver.solve(Problem(INT_SERIES, 5, Ratio(5), true)), Ratio(5, 81));
  check_network(solver.solve(Problem(INT_SERIES, 6, Ratio(6), true)), Ratio(278, 178929));
  check_network(solver.solve(Problem(INT_SERIES, 7, Ratio(7), true)), Ratio(1, 2304));
}

TEST(SolverTest, DefaultParams) {
  test_solver();
}

TEST(SolverTest, TrueParams) {
  test_solver(true);
}

TEST(SolverTest, True3Params) {
  test_solver(true, 3);
}

}  // namespace
}  // namespace network_opt
