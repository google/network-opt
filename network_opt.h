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

#define N Node::create
#define NT Node::create_test
#define WRITEOP(s, op, mathmode) { if (op == '+' && mathmode) s += "$+$"; else s += op; }
#define GET_COST(total, n) (total * total - n)
//#define GET_COST(total, n) (total - PI_RATIO)

using namespace std;
using namespace boost;
using boost::multiprecision::cpp_int;
using Mask = unsigned int;
using Ratio = rational<cpp_int>;
using Value = unsigned int;
using Values = list<Value>;

extern Ratio INT_SERIES[];
extern Ratio E12_SERIES[];
extern Ratio ONE_SERIES[];
Ratio* SERIES = INT_SERIES;
extern Ratio PI_RATIO;

struct Node {
  Values values; Values hidden; list<Node*> children; Ratio ratio;
  static Node& create() { return *(new Node()); }
  static Node& create(Value v) { Node* node = new Node(); node->values.push_back(v); return *node; }
  static Node& create(const Values& vs) { Node* node = new Node(); node->values = vs; return *node; }

  // The following two take care of the "off-by-one" issue in test networks
  // (where we prefer to use literal node values over indices)
  static Node& create_test(Value v) { Node* node = new Node(); node->values.push_back(v - 1); return *node; }
  static Node& create_test(const Values& vs) { Node* node = new Node(); for (auto v : vs) node->values.push_back(v - 1); return *node; }

  ~Node() { for (auto child : children) if (!child->ratio) delete child; }
  Node& operator[](Node& node);
  Node* clone();
  void leafify();
  string to_string(bool mathmode = false, bool top = true, char op1 = '+', char op2 = '|') const;
  string to_network(char op1 = '+', char op2 = '|') const;

 private: Node() {}
};

struct NetworkEvaluator {
  Ratio evaluate_total(const Node* node, int bound = 0, char op1 = '+', char op2 = '|');
  Ratio evaluate_cost(const Node* node, unsigned int n, int bound = 0);
} network_evaluator;

struct Bounder {
  Ratio bound(const Node* network, unsigned int n);
};

struct Expander {
  Expander(Node* n) : network(n) { stack.push_back(n); }
  Node* expandable();
 private: Node* network; list<Node*> stack;
};

struct SubsetCoder {
  void decode(Mask mask, const Values& values, Values& include, Values& exclude);
  Mask encode(const Values& values);
} coder;

struct Tabulator {
  unsigned int m; vector<vector<pair<Ratio, Node*>>> lookup_table;
  Tabulator(unsigned int _m) : m(_m) { }
  ~Tabulator() { clear(); }

  void tabulate(unsigned int n);
  Node* binary_search(const Node* network, Node* expandable, const Values& values, unsigned int n);
  pair<Node*,Node*> linear_search(const Node* network, Node* expandable_0,
      Node* expandable_1, const Values& values_0, const Values& values_1, unsigned int n);

 private:
  void clear();
  void tabulate(unsigned int n, Node* network, Mask mask = 0, Value i = 0);
  void tabulate(vector<pair<Ratio, Node*>>& entry, Node* network);
};

struct Solver {
  Solver(Bounder* b = NULL, Tabulator* t = NULL) :
      bounder(b), tabulator(t), best_network(NULL) {}
  ~Solver() { clear(); }

  Node* solve(unsigned int n);

 private: Bounder* bounder; Tabulator* tabulator; Node* best_network;
  void clear();
  void solve(unsigned int n, Node* network);
};

void print_summary(ostream& os, Node* network, unsigned int n, const string& prefix);

#endif
