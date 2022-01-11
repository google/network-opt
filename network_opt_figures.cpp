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

int main(int argc, char *argv[]) {
  network_opt::Problem problem_7(       network_opt::INT_SERIES,  7, Ratio(7), true);
  network_opt::Problem problem_10(      network_opt::INT_SERIES, 10, network_opt::RATIO_E, false);
  network_opt::Problem problem_12(      network_opt::E12_SERIES, 12, Ratio(12), true);
  network_opt::Problem problem_12_E(    network_opt::INT_SERIES, 12, network_opt::RATIO_E, false);
  network_opt::Problem problem_12_PI(   network_opt::INT_SERIES, 12, network_opt::RATIO_PI, false);
  network_opt::Problem problem_12_PHI(  network_opt::INT_SERIES, 12, network_opt::RATIO_PHI, false);
  network_opt::Problem problem_12_SQRT2(network_opt::INT_SERIES, 12, network_opt::RATIO_SQRT2, false);
  network_opt::Problem problem_15(      network_opt::ONE_SERIES, 15, network_opt::RATIO_PI, false);
  network_opt::Node* network = NULL;

  std::cout << "%%%%%%%% PAGE 3 LEFT TOP %%%%%%%%" << std::endl;
  network = &N()[N()[N()[NT(3)][NT(7)]][N()[NT(1)][NT(2)][NT(6)]][N()[NT(4)][NT(5)]]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_7, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% PAGE 3 RIGHT TOP %%%%%%%%" << std::endl;
  network = &N()[N()[N()[NT(3)][NT(7)]][N()[NT(1)][NT(2)][NT(5)]][N()[NT(4)][NT(6)]]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_7, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% PAGE 3 RIGHT BOTTOM %%%%%%%%" << std::endl;
  network = &N()[NT({1,3})][NT(7)[NT(6)[NT({2,5})]][NT(4)]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_7, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% PAGE 4 LEFT %%%%%%%%" << std::endl;
  network = &N()[NT({1,3})][NT(7)[N(6)[NT({2,5})]][NT(4)]];
  network_opt::visualizer.visualize_tree(std::cout, problem_7, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% PAGE 5 LEFT(A) %%%%%%%%" << std::endl;
  network = &N()[NT({1,3})][NT(7)[N()[NT({2,5,6})]][NT(4)]];
  network_opt::visualizer.visualize_tree(std::cout, problem_7, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% PAGE 5 LEFT(B) %%%%%%%%" << std::endl;
  network = &N()[N()[NT(1)][NT(3)][NT(4)]][NT(7)[N()[NT({2,5,6})]]];
  network_opt::visualizer.visualize_tree(std::cout, problem_7, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% PAGE 5 RIGHT TOP %%%%%%%%" << std::endl;
  network_opt::Tabulator tabulator(3);
  tabulator.tabulate(problem_7);
  int entry256 = network_opt::coder.encode({1,4,5});
  int entry134 = network_opt::coder.encode({0,2,3});
  std::cout << "\\begin{table}[t]" << std::endl;
  std::cout << "\\begin{center}" << std::endl;
  std::cout << "\\small" << std::endl;
  std::cout << "\\begin{tabular}{|cr|c|cr|}\\cline{1-2}\\cline{4-5}" << std::endl;
  std::cout << "Subcircuit & Resistance & & ";
  std::cout << "Subcircuit & Resistance \\\\\\cline{1-2}\\cline{4-5}" << std::endl;
  for (int i = 0; i < 8; ++i) {
    auto subcircuit256 = tabulator.lookup_table[entry256][i];
    auto subcircuit134 = tabulator.lookup_table[entry134][i];
    std::cout << "$" << subcircuit256.second->to_string(problem_7, true) << "$ & $";
    if (subcircuit256.first.denominator() == 1) std::cout << subcircuit256.first.numerator();
    else std::cout << "\\sfrac{" << subcircuit256.first.numerator() << "}{" << subcircuit256.first.denominator() << "}";
    std::cout << "\\ \\Omega \\quad$ & & ";
    std::cout << "$" << subcircuit134.second->to_string(problem_7, true) << "$ & $";
    if (subcircuit134.first.denominator() == 1) std::cout << subcircuit134.first.numerator();
    else std::cout << "\\sfrac{" << subcircuit134.first.numerator() << "}{" << subcircuit134.first.denominator() << "}";
    std::cout << "\\ \\Omega \\quad$\\\\" << std::endl;
  }
  std::cout << "\\cline{1-2}\\cline{4-5}" << std::endl;
  std::cout << "\\multicolumn{2}{c}{(a) $\\mathcal{C}(\\{2,5,6\\})$} & \\multicolumn{1}{c}{} & ";
  std::cout << "\\multicolumn{2}{c}{(b) $\\mathcal{C}(\\{1,3,4\\})$}" << std::endl;
  std::cout << "\\end{tabular}" << std::endl;
  std::cout << "\\caption{Subcircuit configurations sorted by resistance.}" << std::endl;
  std::cout << "\\label{configs}" << std::endl;
  std::cout << "\\end{center}" << std::endl;
  std::cout << "\\end{table}" << std::endl;
  std::cout << std::endl;

  std::cout << "%%%%%%%% PAGE 5 RIGHT BOTTOM %%%%%%%%" << std::endl;
  network = &N()[N()[NT({2,5,6})]][NT(7)][N()[NT({1,3,4})]];
  network_opt::visualizer.visualize_tree(std::cout, problem_7, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% PAGE 7 (LEFT) %%%%%%%%" << std::endl;
  network = &N()[N({0,5})][N()[N()[N(1)][N()[N(7)[N()[N(9)[N({2,11})]][N({4,8})]]]]][N()[N(3)][N({6,10})]]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_12, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% PAGE 7 (RIGHT) %%%%%%%%" << std::endl;
  network = &N()[N(0)][N(1)][N()[N()[N(2)][N(3)][N(4)][N(5)][N()[N(6)][N()[N(7)][N()[N()[N(8)][N(9)][N({10,11})]]]]]][N()[N(12)][N({13,14})]]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_15, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% FRIEDMAN (10-resistor of e, *previous*) %%%%%%%%" << std::endl;
  network = &N()[N()[NT(5)][N()[N()[NT(8)][N()[NT(7)][N()[NT(10)][NT({3,4})]]]][N()[NT(2)][NT(6)][NT({1,9})]]]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_10, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% FRIEDMAN (10-resistor of e) %%%%%%%%" << std::endl;
  network = &N()[N(0)][N()[N(1)][N(4)[N(2)][N(5)[N()[N()[N(9)[N(7)[N()[N(3)][N({6,8})]]]]]]]]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_10, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% FRIEDMAN (12-resistor of pi) %%%%%%%%" << std::endl;
  network = &N()[N(10)[N()[N()[N()[N(6)[N(4)[N({0,1})]]]]][N()[N()[N(11)[N()[N(2)][N({3,8})]]]]]][N(7)[N({5,9})]]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_12_PI, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% FRIEDMAN (12-resistor of e) %%%%%%%%" << std::endl;
  network = &N()[N(11)[N()[N()[N(6)[N({0,5})]]]][N(2)[N(9)[N()[N()[N()[N(1)][N()[N(3)][N(8)[N()[N(4)][N({7,10})]]]]]]]]]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_12_E, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% FRIEDMAN (12-resistor of phi) %%%%%%%%" << std::endl;
  network = &N()[N(8)[N()[N()[N()[N(4)[N(0)][N()[N(1)][N({6,7})]]]]][N()[N()[N({5,10})]]]][N(11)[N(2)][N(9)]][N(3)]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_12_PHI, network);
  std::cout << std::endl;
  delete network;

  std::cout << "%%%%%%%% FRIEDMAN (12-resistor of sqrt(2)) %%%%%%%%" << std::endl;
  network = &N()[N()[N()[N()[N()[N()[N(0)][N(9)[N(3)[N(11)[N({2,7})]]]]]]][N()[N()[N(10)[N(1)][N(5)]]]]][N(4)][N({6,8})]];
  network_opt::visualizer.visualize_schematic(std::cout, problem_12_SQRT2, network);
  std::cout << std::endl;
  delete network;
}
