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
  Node* network = NULL;

  cout << "%%%%%%%% PAGE 3 LEFT TOP %%%%%%%%" << endl;
  network = &N()[N()[N()[NT(3)][NT(7)]][N()[NT(1)][NT(2)][NT(6)]][N()[NT(4)][NT(5)]]];
  visualizer.visualize_schematic(cout, network, 7);
  cout << endl;
  delete network;

  cout << "%%%%%%%% PAGE 3 RIGHT TOP %%%%%%%%" << endl;
  network = &N()[N()[N()[NT(3)][NT(7)]][N()[NT(1)][NT(2)][NT(5)]][N()[NT(4)][NT(6)]]];
  visualizer.visualize_schematic(cout, network, 7);
  cout << endl;
  delete network;

  cout << "%%%%%%%% PAGE 3 RIGHT BOTTOM %%%%%%%%" << endl;
  network = &N()[NT({1,3})][NT(7)[NT(6)[NT({2,5})]][NT(4)]];
  visualizer.visualize_schematic(cout, network, 7);
  cout << endl;
  delete network;

  cout << "%%%%%%%% PAGE 4 LEFT %%%%%%%%" << endl;
  network = &N()[NT({1,3})][NT(7)[N(6)[NT({2,5})]][NT(4)]];
  visualizer.visualize_tree(cout, network, 7);
  cout << endl;
  delete network;

  cout << "%%%%%%%% PAGE 5 LEFT(A) %%%%%%%%" << endl;
  network = &N()[NT({1,3})][NT(7)[N()[NT({2,5,6})]][NT(4)]];
  visualizer.visualize_tree(cout, network, 7);
  cout << endl;
  delete network;

  cout << "%%%%%%%% PAGE 5 LEFT(B) %%%%%%%%" << endl;
  network = &N()[N()[NT(1)][NT(3)][NT(4)]][NT(7)[N()[NT({2,5,6})]]];
  visualizer.visualize_tree(cout, network, 7);
  cout << endl;
  delete network;

  cout << "%%%%%%%% PAGE 5 RIGHT TOP %%%%%%%%" << endl;
  Tabulator tabulator(3);
  tabulator.tabulate(7);
  int entry256 = coder.encode({1,4,5});
  int entry134 = coder.encode({0,2,3});
  cout << "\\begin{table}[t]" << endl;
  cout << "\\begin{center}" << endl;
  cout << "\\small" << endl;
  cout << "\\begin{tabular}{|cr|c|cr|}\\cline{1-2}\\cline{4-5}" << endl;
  cout << "Subcircuit & Resistance & & ";
  cout << "Subcircuit & Resistance \\\\\\cline{1-2}\\cline{4-5}" << endl;
  for (int i = 0; i < 8; ++i) {
    auto subcircuit256 = tabulator.lookup_table[entry256][i];
    auto subcircuit134 = tabulator.lookup_table[entry134][i];
    cout << "$" << subcircuit256.second->to_string(true) << "$ & $";
    if (subcircuit256.first.denominator() == 1) cout << subcircuit256.first.numerator();
    else cout << "\\sfrac{" << subcircuit256.first.numerator() << "}{" << subcircuit256.first.denominator() << "}";
    cout << "\\ \\Omega \\quad$ & & ";
    cout << "$" << subcircuit134.second->to_string(true) << "$ & $";
    if (subcircuit134.first.denominator() == 1) cout << subcircuit134.first.numerator();
    else cout << "\\sfrac{" << subcircuit134.first.numerator() << "}{" << subcircuit134.first.denominator() << "}";
    cout << "\\ \\Omega \\quad$\\\\" << endl;
  }
  cout << "\\cline{1-2}\\cline{4-5}" << endl;
  cout << "\\multicolumn{2}{c}{(a) $\\mathcal{C}(\\{2,5,6\\})$} & \\multicolumn{1}{c}{} & ";
  cout << "\\multicolumn{2}{c}{(b) $\\mathcal{C}(\\{1,3,4\\})$}" << endl;
  cout << "\\end{tabular}" << endl;
  cout << "\\caption{Subcircuit configurations sorted by resistance.}" << endl;
  cout << "\\label{configs}" << endl;
  cout << "\\end{center}" << endl;
  cout << "\\end{table}" << endl;
  cout << endl;

  cout << "%%%%%%%% PAGE 5 RIGHT BOTTOM %%%%%%%%" << endl;
  network = &N()[N()[NT({2,5,6})]][NT(7)][N()[NT({1,3,4})]];
  visualizer.visualize_tree(cout, network, 7);
  cout << endl;
  delete network;

  cout << "%%%%%%%% PAGE 7 (LEFT) %%%%%%%%" << endl;
  SERIES = E12_SERIES;
  network = &N()[N({0,5})][N()[N()[N(1)][N()[N(7)[N()[N(9)[N({2,11})]][N({4,8})]]]]][N()[N(3)][N({6,10})]]];
  visualizer.visualize_schematic(cout, network, 12);
  cout << endl;
  delete network;

  cout << "%%%%%%%% PAGE 7 (RIGHT) %%%%%%%%" << endl;
  SERIES = ONE_SERIES;
  network = &N()[N(0)][N(1)][N()[N()[N(2)][N(3)][N(4)][N(5)][N()[N(6)][N()[N(7)][N()[N()[N(8)][N(9)][N({10,11})]]]]]][N()[N(12)][N({13,14})]]];
  visualizer.visualize_schematic(cout, network, 15);
  cout << endl;
  delete network;
}
