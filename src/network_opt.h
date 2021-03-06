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

#ifndef _NETWORK_OPT_H_
#define _NETWORK_OPT_H_

#include <algorithm>
#include <assert.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <iostream>
#include <list>
#include <math.h>
#include <stdlib.h>
#include <set>
#include <string>
#include <vector>

#define N network_opt::Node::create
#define NT network_opt::Node::create_test

using boost::multiprecision::cpp_int;
using Mask = unsigned int;
using Ratio = boost::rational<cpp_int>;
using Value = unsigned int;
using Values = std::list<Value>;

namespace network_opt {

extern Ratio RATIO_E;
extern Ratio RATIO_PI;
extern Ratio RATIO_PHI;
extern Ratio RATIO_SQRT2;

extern Ratio INT_SERIES[];
extern Ratio E12_SERIES[];
extern Ratio ONE_SERIES[];

struct Problem {
  std::vector<Ratio> elements;
  Ratio target;
  bool square;

  static Problem from_argv(char* argv[]);

  Problem(const Ratio* series, unsigned int n, const Ratio& t, bool s);
  unsigned int size() const;
  const Ratio& operator[](unsigned int idx) const;
  Ratio get_cost(const Ratio& total) const;
};

struct Node {
  Values values; Values hidden; std::list<Node*> children; Ratio ratio;
  static Node& create();
  static Node& create(Value v);
  static Node& create(const Values& vs);

  // The following two take care of the "off-by-one" issue in test networks
  // (where we prefer to use literal node values over indices)
  static Node& create_test(Value v);
  static Node& create_test(const Values& vs);

  ~Node();
  Node& operator[](Node& node);
  Node* clone();
  void leafify();
  std::string to_string(const Problem& problem, bool mathmode = false, bool top = true, char op1 = '+', char op2 = '|') const;
  std::string to_network(char op1 = '+', char op2 = '|') const;

 private: Node() {}
};

struct NetworkEvaluator {
  Ratio evaluate_total(const Problem& problem, const Node* node, int bound = 0, char op1 = '+', char op2 = '|');
  Ratio evaluate_cost(const Problem& problem, const Node* node, int bound = 0);
};

extern NetworkEvaluator network_evaluator;

struct Bounder {
  Ratio bound(const Problem& problem, const Node* network);
};

struct Expander {
  Expander(Node* n) : network(n) { stack.push_back(n); }
  Node* expandable();
 private: Node* network; std::list<Node*> stack;
};

struct SubsetCoder {
  void decode(Mask mask, const Values& values, Values& include, Values& exclude);
  Mask encode(const Values& values);
};

extern SubsetCoder coder;

struct Tabulator {
  unsigned int m; std::vector<std::vector<std::pair<Ratio, Node*>>> lookup_table;
  Tabulator(unsigned int _m) : m(_m) { }
  ~Tabulator() { clear(); }

  void tabulate(const Problem& problem);
  Node* binary_search(const Problem& problem, const Node* network, Node* expandable, const Values& values);
  std::pair<Node*,Node*> linear_search(const Problem& problem, const Node* network, Node* expandable_0,
      Node* expandable_1, const Values& values_0, const Values& values_1);

 private:
  void clear();
  void tabulate(const Problem& problem, Node* network, Mask mask = 0, Value i = 0);
  void tabulate(const Problem& problem, Node* network, std::vector<std::pair<Ratio, Node*>>& entry);
};

struct Params {
  bool b;
  unsigned int m;
  Params(bool _b, unsigned int _m) : b(_b), m(_m) {}
};

struct Solver {
  Solver(const Params& params);
  ~Solver();
  Node* solve(const Problem& problem);

 private: Bounder* bounder; Tabulator* tabulator; Node* best_network;
  void clear();
  void solve(const Problem& problem, Node* network);
};

void print_summary(std::ostream& os, const Problem& problem, Node* network, const std::string& prefix);

}

#endif
