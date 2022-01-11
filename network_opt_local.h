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

#ifndef _NETWORK_OPT_LOCAL_H_
#define _NETWORK_OPT_LOCAL_H_

#include "network_opt.h"

namespace network_opt {

struct LocalSolver {
  LocalSolver(const Params& params);
  ~LocalSolver();
  Node* solve(const Problem& problem);

 private:
  Bounder* bounder;
  Tabulator* tabulator;
  Node* best_network;
  std::vector<Node*> expandables;

  void clear();
  void randomly_expand(Node* node);
  void iteratively_improve(const Problem& problem, Node* network);
};
    
}

#endif
