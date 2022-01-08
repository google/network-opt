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

#ifndef _NETWORK_OPT_UTILS_H_
#define _NETWORK_OPT_UTILS_H_

#include "network_opt.h"

namespace network_opt {

struct Visual {
  Ratio x, y, w, h; Value v; std::vector<Visual> subvisuals;
  Visual(Ratio _x = 0, Ratio _y = 0, Ratio _w = 0, Ratio _h = 0, Value _v = INT_MAX) :
    x(_x), y(_y), w(_w), h(_h), v(_v) {}
};

struct Visualizer {
  void visualize_schematic(std::ostream& os, const Problem& problem, Node* network);
  void visualize_tree(std::ostream& os, const Problem& problem, Node* network);

 private:
  Ratio get_width();
  void output_coord(std::ostream& os, const Ratio& begin, const Ratio& end, const std::string& s = "");
  void calc_area(Node* node, Visual& visual, char op1 = '+', char op2 = '|');
  void update_coords(Visual& visual, const Ratio& _x = 0, const Ratio& _y = 0, char op1 = '+', char op2 = '|');
  void flip(Visual& visual, Ratio height);
  void visualize_schematic(std::ostream& os, const Problem& problem, Visual& visual, char op1 = '+', char op2 = '|');
  void visualize_tree(std::ostream& os, Node* node, char op1, char op2);
} visualizer;

}

#endif
