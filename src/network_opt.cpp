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

#define WRITEOP(s, op, mathmode) { if (op == '+' && mathmode) s += "$+$"; else s += op; }

namespace network_opt {

Ratio RATIO_E     = Ratio(271828182845905,100000000000000);
Ratio RATIO_PI    = Ratio(314159265358979,100000000000000);
Ratio RATIO_PHI   = Ratio(161803398874989,100000000000000);
Ratio RATIO_SQRT2 = Ratio(141421356237309,100000000000000);

Ratio INT_SERIES[] = {Ratio( 1), Ratio( 2), Ratio( 3), Ratio( 4),
                      Ratio( 5), Ratio( 6), Ratio( 7), Ratio( 8),
                      Ratio( 9), Ratio(10), Ratio(11), Ratio(12)};
Ratio ODD_SERIES[] = {Ratio( 1), Ratio( 3), Ratio( 5), Ratio( 7),
                      Ratio( 9), Ratio(11), Ratio(13), Ratio(15),
                      Ratio(17), Ratio(19), Ratio(21), Ratio(23)};
Ratio EVEN_SERIES[] = {Ratio(2), Ratio( 4), Ratio( 6), Ratio( 8),
                      Ratio(10), Ratio(12), Ratio(14), Ratio(16),
                      Ratio(18), Ratio(20), Ratio(22), Ratio(24)};
Ratio E12_SERIES[] = {Ratio(10,10), Ratio(12,10), Ratio(15,10), Ratio(18,10),
                      Ratio(22,10), Ratio(27,10), Ratio(33,10), Ratio(39,10),
                      Ratio(47,10), Ratio(56,10), Ratio(68,10), Ratio(82,10)};
Ratio ONE_SERIES[] = {Ratio(1), Ratio(1), Ratio(1), Ratio(1),
                      Ratio(1), Ratio(1), Ratio(1), Ratio(1),
                      Ratio(1), Ratio(1), Ratio(1), Ratio(1),
                      Ratio(1), Ratio(1), Ratio(1), Ratio(1)};

Problem Problem::from_argv(char* argv[]) {
  unsigned int n = atoi(argv[4]);
  std::string s = argv[5], g = argv[6];
  Ratio* series = E12_SERIES;
  Ratio target = Ratio(n);
  bool square = true;
  if (s ==   "INT") series = INT_SERIES;
  if (s ==   "ODD") series = ODD_SERIES;
  if (s ==  "EVEN") series = EVEN_SERIES;
  if (s ==   "ONE") series = ONE_SERIES;
  if (g ==     "E") { square = false; target = RATIO_E; }
  if (g ==    "PI") { square = false; target = RATIO_PI; }
  if (g ==   "PHI") { square = false; target = RATIO_PHI; }
  if (g == "SQRT2") { square = false; target = RATIO_SQRT2; }
  return Problem(series, n, target, square);
}

Problem::Problem(const Ratio* series, unsigned int n, const Ratio& t, bool s) {
  for (unsigned int i = 0; i < n; i++) elements.push_back(series[i]);
  target = t;
  square = s;
}

unsigned int Problem::size() const { return elements.size(); }

const Ratio& Problem::operator[](unsigned int idx) const { return elements[idx]; }

Ratio Problem::get_cost(const Ratio& total) const {
  return (square ? total * total : total) - target;
}

Node& Node::create() { return *(new Node()); }

Node& Node::create(Value v) { Node* node = new Node(); node->values.push_back(v); return *node; }

Node& Node::create(const Values& vs) { Node* node = new Node(); node->values = vs; return *node; }

Node& Node::create_test(Value v) { Node* node = new Node(); node->values.push_back(v - 1); return *node; }

Node& Node::create_test(const Values& vs) { Node* node = new Node(); for (auto v : vs) node->values.push_back(v - 1); return *node; }

Node::~Node() { for (auto child : children) if (!child->ratio) delete child; }

Node& Node::operator[](Node& node) { children.push_back(&node); return *this; }

Node* Node::clone() {
  if (ratio) return this;
  Node* my_clone = &N(values);
  for (auto child : children) my_clone->children.push_back(child->clone());
  return my_clone;
}

void Node::leafify() {
  if (children.empty()) {
    if (values.size() == 2) {
      while (!values.empty()) {
        children.push_back(&N(values.front()));
        values.pop_front();
      }
    }
  } else {
    for (auto child : children) child->leafify();
    if (!values.empty()) {
      children.push_back(&N(values));
      values.clear();
    }
  }
}

std::string Node::to_string(const Problem& problem, bool mathmode, bool top, char op1, char op2) const {
  std::string s = "";
  if (!top && values.size() + children.size() > 1) s += "(";
  for (auto& child : children) {
    if (child != *children.begin()) WRITEOP(s, op1, mathmode);
    bool subtop = top && values.size() == 0 && children.size() == 1;
    s += child->ratio ? child->to_string(problem, mathmode, subtop)
                      : child->to_string(problem, mathmode, subtop, op2, op1);
  }
  if (!values.empty() && !children.empty()) WRITEOP(s, op1, mathmode);
  for (auto& value : values) {
    if (value != *values.begin()) WRITEOP(s, op1, mathmode);
    auto v = boost::rational_cast<long long>(problem[value] * 10);
    s += std::to_string(v / 10);
    if (v % 10) s += "." + std::to_string(v % 10);
  }
  if (!top && values.size() + children.size() > 1) s += ")";
  return s;
}

std::string Node::to_network(char op1, char op2) const {
  std::string s = "N(";
  if (values.size() > 1) s += "{";
  for (auto& value : values) {
    if (value != *values.begin()) s += ",";
    s += std::to_string(value);
  }
  if (values.size() > 1) s += "}";
  s += ")";
  for (auto& child : children) {
    s += "[";
    if (child->ratio && op1 == '+') s += "N()[";
    s += child->ratio ? child->to_network() : child->to_network(op2, op1);
    if (child->ratio && op1 == '+') s += "]";
    s += "]";
  }
  return s;
}

Ratio NetworkEvaluator::evaluate_total(const Problem& problem, const Node* node, int bound, char op1, char op2) {
  char op = (bound == 1) ? '+' : '|';
  Ratio result;
  if (!node->values.empty()) {
    Ratio subresult;
    char valueop = (node->values.size() > 2 || !node->children.empty()) ? op : op1;
    for (auto value : node->values) {
      subresult += (valueop == '+') ? problem[value] : 1 / problem[value];
    }
    result += (valueop == '+') ? subresult : 1 / subresult;
    if (op1 == '|') result = 1 / result;
  }
  for (auto& child : node->children) {
    Ratio subresult = child->ratio ? child->ratio : evaluate_total(problem, child, bound, op2, op1);
    result += (op1 == '+') ? subresult : 1 / subresult;
  }
  return (op1 == '+') ? result : 1 / result;
}

Ratio NetworkEvaluator::evaluate_cost(const Problem& problem, const Node* node, int bound) {
  Ratio total = evaluate_total(problem, node, bound);
  Ratio cost = problem.get_cost(total);
  return (cost > 0) ? cost : -cost;
}

NetworkEvaluator network_evaluator;

Ratio Bounder::bound(const Problem& problem, const Node* network) {
  Ratio lower_bound = network_evaluator.evaluate_total(problem, network, -1);
  Ratio upper_bound = network_evaluator.evaluate_total(problem, network,  1);
  return max( problem.get_cost(lower_bound),
             -problem.get_cost(upper_bound));
}

Node* Expander::expandable() {
  while (!stack.empty()) {
    Node* node = stack.back(); stack.pop_back();
    for (auto child : node->children) stack.push_back(child);
    if (node->values.size() > 2) return node;
    if (node->values.size() > 1)
      if (!node->children.empty() || node == network) return node;
  }
  return NULL;
}

void SubsetCoder::decode(Mask mask, const Values& values, Values& include, Values& exclude) {
  for (auto value : values) {
    if (include.empty()) { include.push_back(value); continue; }
    if (mask & 0x1) include.push_back(value);
    else exclude.push_back(value);
    mask >>= 1;
  }
}

Mask SubsetCoder::encode(const Values& values) {
  Mask mask = 0;
  for (auto value : values) mask |= 1 << value;
  return mask;
}

SubsetCoder coder;

void Tabulator::tabulate(const Problem& problem) {
  clear();
  lookup_table.resize(1 << problem.size());
  Node* network = &N();
  tabulate(problem, network);
  delete network;
}

Node* Tabulator::binary_search(const Problem& problem, const Node* network, Node* expandable, const Values& values) {
  Mask mask = coder.encode(values);
  std::vector<std::pair<Ratio, Node*>>& entry = lookup_table[mask];
  int lo = 0, hi = entry.size(), best_idx = -1;
  Ratio best_cost = -1;
  while (lo < hi) {
    int mid = (lo + hi) / 2;
    expandable->children.push_back(entry[mid].second);
    Ratio total = network_evaluator.evaluate_total(problem, network);
    Ratio cost = problem.get_cost(total);
    Ratio abs_cost = (cost > 0) ? cost : -cost;
    if (best_cost < 0 || best_cost > abs_cost) {
      best_cost = abs_cost; best_idx = mid;
    }
    if (cost < 0) lo = mid + 1; else hi = mid;
    expandable->children.pop_back();
  }
  return entry[best_idx].second;
}

std::pair<Node*,Node*> Tabulator::linear_search(const Problem& problem, const Node* network, Node* expandable_0,
    Node* expandable_1, const Values& values_0, const Values& values_1) {
  Mask mask_0 = coder.encode(values_0), mask_1 = coder.encode(values_1);
  std::vector<std::pair<Ratio, Node*>>& entry_0 = lookup_table[mask_0],
                                        entry_1 = lookup_table[mask_1];
  unsigned int lo = 0;
  int hi = entry_1.size() - 1, best_lo = -1, best_hi = -1;
  Ratio best_cost = -1;
  while (lo < entry_0.size() && hi >= 0) {
    expandable_0->children.push_back(entry_0[lo].second);
    expandable_1->children.push_back(entry_1[hi].second);
    Ratio total = network_evaluator.evaluate_total(problem, network);
    Ratio cost = problem.get_cost(total);
    Ratio abs_cost = (cost > 0) ? cost : -cost;
    if (best_cost < 0 || best_cost > abs_cost) {
      best_cost = abs_cost; best_lo = lo; best_hi = hi;
    }
    if (cost < 0) lo += 1; else hi -= 1;
    expandable_1->children.pop_back();
    expandable_0->children.pop_back();
  }
  return std::pair<Node*,Node*>(entry_0[best_lo].second, entry_1[best_hi].second);
}

void Tabulator::clear() {
  for (auto entries : lookup_table) for (auto entry : entries) delete entry.second;
  lookup_table.clear();
}

void Tabulator::tabulate(const Problem& problem, Node* network, Mask mask, Value i) {
  if (i >= problem.size()) {
    if (mask) {
      std::vector<std::pair<Ratio, Node*>>& entry = lookup_table[mask];
      tabulate(problem, network, entry);
      sort(entry.begin(), entry.end());
    }
    return;
  }
  tabulate(problem, network, mask, i + 1);
  if (network->values.size() < m) {
    network->values.push_back(i);
    tabulate(problem, network, mask | (1 << i), i + 1);
    network->values.pop_back();
  }
}

void Tabulator::tabulate(const Problem& problem, Node* network, std::vector<std::pair<Ratio, Node*>>& entry) {
  Expander expander(network);
  Node* expandable = expander.expandable();
  if (!expandable) {
    Node* clone = network->clone();
    clone->ratio = network_evaluator.evaluate_total(problem, network);
    entry.push_back(std::pair<Ratio, Node*>(clone->ratio, clone));
    return;
  }
  Values values = expandable->values; expandable->values.clear();
  bool has_children = !expandable->children.empty();
  Node* child = &N();
  expandable->children.push_back(child);
  Mask max_mask = 1 << (values.size() - 1);
  for (Mask mask = 0; mask < max_mask; ++mask) {
    coder.decode(mask, values, child->values, expandable->values);
    if (has_children || !expandable->values.empty() || expandable == network)
      tabulate(problem, network, entry);
    child->values.clear();
    expandable->values.clear();
  }
  expandable->children.pop_back();
  delete child;
  expandable->values = values;
}

Solver::Solver(const Params& params) : bounder(NULL), tabulator(NULL), best_network(NULL) {
  if (params.b) bounder = new Bounder();
  if (params.m) tabulator = new Tabulator(params.m);
}

Solver::~Solver() {
  clear();
  if (tabulator) delete tabulator;
  if (bounder) delete bounder;
}

Node* Solver::solve(const Problem& problem) {
  clear();
  Node* network = &N();
  for (Value i = 0; i < problem.size(); ++i) network->values.push_back(i);
  if (tabulator) tabulator->tabulate(problem);
  solve(problem, network);
  delete network;
  return best_network;
}

void Solver::clear() {
  if (best_network) delete best_network;
  best_network = NULL;
}

void Solver::solve(const Problem& problem, Node* network) {
  if (bounder && best_network && bounder->bound(problem, network) >= best_network->ratio)
    return;
  Expander expander(network);
  Node* expandable_0 = expander.expandable();
  if (!expandable_0) {
    Ratio cost = network_evaluator.evaluate_cost(problem, network);
    if (!best_network || best_network->ratio > cost) {
      if (best_network) delete best_network;
      best_network = network->clone();
      best_network->ratio = cost;
    }
    return;
  }
  Node* expandable_1 = expander.expandable();
  Node* expandable_2 = expandable_1 ? expander.expandable() : NULL;
  Values values_0 = expandable_0->values; expandable_0->values.clear();
  if (tabulator && !expandable_1 && values_0.size() <= tabulator->m) {
    Node* node = tabulator->binary_search(problem, network, expandable_0, values_0);
    expandable_0->children.push_back(node);
    solve(problem, network);
    expandable_0->children.pop_back();
  } else if (tabulator && expandable_1 && !expandable_2 &&
             values_0.size() <= tabulator->m &&
             expandable_1->values.size() <= tabulator->m) {
    Values values_1 = expandable_1->values; expandable_1->values.clear();
    std::pair<Node*,Node*> nodes = tabulator->linear_search(
        problem, network, expandable_0, expandable_1, values_0, values_1);
    expandable_0->children.push_back(nodes.first);
    expandable_1->children.push_back(nodes.second);
    solve(problem, network);
    expandable_1->children.pop_back();
    expandable_0->children.pop_back();
    expandable_1->values = values_1;
  } else {
    bool has_children = !expandable_0->children.empty();
    Node* child = &N();
    expandable_0->children.push_back(child);
    Mask max_mask = 1 << (values_0.size() - 1);
    for (Mask mask = 0; mask < max_mask; ++mask) {
      coder.decode(mask, values_0, child->values, expandable_0->values);
      if (has_children || !expandable_0->values.empty() || expandable_0 == network)
        solve(problem, network);
      child->values.clear();
      expandable_0->values.clear();
    }
    expandable_0->children.pop_back();
    delete child;
  }
  expandable_0->values = values_0;
}

void print_summary(std::ostream& os, const Problem& problem, Node* network, const std::string& prefix) {
  Ratio total = network_evaluator.evaluate_total(problem, network);
  double cost = boost::rational_cast<double>(problem.get_cost(total));
  double target = boost::rational_cast<double>(problem.target);
  if (problem.square) {
    target = std::sqrt(target);
    cost = boost::rational_cast<double>(total) - target;
  }
  os << prefix << "Solution: " << network->to_string(problem) << std::endl;
  os << prefix << " Network: " << network->to_network() << std::endl;
  os << std::setprecision(16);
  os << prefix << "  Target: " << target << std::endl;
  os << prefix << "   Total: " << boost::rational_cast<double>(total) << " (" << total << ")" << std::endl;
  os << std::setprecision(4);
  os << prefix << "    Cost: " << cost << std::endl;
}

}
