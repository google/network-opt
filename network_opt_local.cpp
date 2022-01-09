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
#include <algorithm>
#include <chrono>
#include "stdlib.h"

namespace network_opt {

struct LocalSolver {
  LocalSolver(const Params) :
      bounder(b), tabulator(t), best_network(NULL) {}
  ~LocalSolver() { clear(); }

  Node* solve(const Problem& problem) {
    auto start = std::chrono::steady_clock::now();
    clear();
    Ratio best_cost = 0;
    if (tabulator) tabulator->tabulate(problem);
    while (true) {
      expandables.clear();
      std::vector<Value> values;
      for (Value i = 0; i < problem.size(); ++i) values.push_back(i);
      random_shuffle(values.begin(), values.end());
      Node* network = &N();
      for (auto value : values) network->values.push_back(value);
      randomly_expand(network);
      iteratively_improve(problem, network);
      Ratio cost = network_evaluator.evaluate_cost(problem, network);
      if (best_network == NULL || best_cost > cost) {
        clear();
        best_cost = cost;
        best_network = network->clone();
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        std::cout << "Found after " << duration.count() << " seconds: " << std::endl;
        print_summary(std::cout, problem, best_network, "");
        std::cout << std::endl;
      }
      delete network;
    }
    return best_network;
  }

 private:
  Bounder* bounder;
  Tabulator* tabulator;
  Node* best_network;
  std::vector<Node*> expandables;

  void clear() {
    if (best_network) delete best_network;
    best_network = NULL;
  }

  void randomly_expand(Node* node) {
    if (node->values.size() <= tabulator->m) {
      expandables.push_back(node);
      node->hidden = node->values;
      node->values.clear();
      Mask mask = coder.encode(node->hidden);
      std::vector<std::pair<Ratio, Node*>>& entry = tabulator->lookup_table[mask];
      unsigned int idx = rand() % entry.size();
      node->children.push_back(entry[idx].second);
      return;
    }
    for (Value v : node->values) {
      unsigned int idx = rand() % (node->children.size() + 1);
      if (idx == node->children.size()) node->children.push_back(&N());
      auto child = node->children.begin();
      for (unsigned int i = 0; i < idx; i++) child++;
      (*child)->values.push_back(v);
    }
    node->values.clear();
    for (auto child : node->children) randomly_expand(child);
  }

  void iteratively_improve(const Problem& problem, Node* network) {
    Ratio best_cost = network_evaluator.evaluate_cost(problem, network);
    while (true) {
      int idx_0 = rand() % expandables.size();
      int idx_1 = rand() % expandables.size();
      if (idx_0 == idx_1) {
        Node* expandable = expandables[idx_0];
        expandable->children.clear();
        Node* node = tabulator->binary_search(
            problem, network, expandable, expandable->hidden);
        expandable->children.push_back(node);
      } else {
        Node* expandable_0 = expandables[idx_0];
        Node* expandable_1 = expandables[idx_1];
        expandable_0->children.clear();
        expandable_1->children.clear();
        std::pair<Node*,Node*> nodes = tabulator->linear_search(
          problem, network, expandable_0, expandable_1, expandable_0->hidden,
          expandable_1->hidden);
        expandable_0->children.push_back(nodes.first);
        expandable_1->children.push_back(nodes.second);
      }
      Ratio cost = network_evaluator.evaluate_cost(problem, network);
      if (best_cost <= cost) break;
      best_cost = cost;
    }
  }
};
    
}

int main(int argc, char *argv[]) {
  srand(2022);
  std::cout << " Command:";
  for (int i = 0; i < argc; ++i) std::cout << " " << argv[i];
  std::cout << std::endl;
  unsigned int b = atoi(argv[1]), t = atoi(argv[2]);
  network_opt::Problem problem = network_opt::Problem::from_argv(argv);
  network_opt::Params params(b, t);
  network_opt::LocalSolver solver(params);
  network_opt::Node* network = solver.solve(problem);
  print_summary(std::cout, problem, network, "");
}
