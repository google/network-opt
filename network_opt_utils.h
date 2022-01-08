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

const Ratio ONE_HALF = Ratio(1, 2);
const Ratio ONE_QUARTER = Ratio(1, 4);

namespace network_opt {

struct Visual {
  Ratio x, y, w, h; Value v; std::vector<Visual> subvisuals;
  Visual(Ratio _x = 0, Ratio _y = 0, Ratio _w = 0, Ratio _h = 0, Value _v = INT_MAX) :
    x(_x), y(_y), w(_w), h(_h), v(_v) {}
};

struct Visualizer {
  void visualize_schematic(std::ostream& os, const Problem& problem, Node* network) {
    Visual visual;
    Node* leafified = network->clone(); leafified->leafify();
    calc_area(leafified, visual);
    update_coords(visual, ONE_QUARTER);
    flip(visual, visual.h);
    print_summary(os, problem, network, "% ");
    os << "\\begin{center}" << std::endl;
    os << "\\small" << std::endl;
    os << "\\ctikzset{bipoles/resistor/height=0.1}" << std::endl;
    os << "\\ctikzset{bipoles/resistor/width=0.4}" << std::endl;
    os << "\\begin{tikzpicture}[scale=0.75]" << std::endl;
    os << "\\draw[color=black]" << std::endl;
    output_coord(os,           0, (visual.h - ONE_HALF), " to ");
    os << "[short,*-] ";
    output_coord(os, ONE_QUARTER, (visual.h - ONE_HALF));
    visualize_schematic(os, visual);
    output_coord(os, (visual.w + ONE_QUARTER), (visual.h - ONE_HALF), " to ");
    os << "[short,-*] ";
    output_coord(os, (visual.w + ONE_HALF   ), (visual.h - ONE_HALF));
    os << ";" << std::endl;
    os << "\\end{tikzpicture}" << std::endl;
    os << "\\end{center}" << std::endl;
    delete leafified;
  }

  void visualize_tree(std::ostream& os, const Problem& problem, Node* network) {
    Node* leafified = network->clone(); leafified->leafify();
    print_summary(os, problem, network, "% ");
    os << "\\begin{center}" << std::endl;
    os << "\\small" << std::endl;
    os << "\\begin{forest}" << std::endl;
    os << "for tree={grow'=0,draw}" << std::endl;
    visualize_tree(os, leafified, '+', '|');
    os << std::endl;
    os << "\\end{forest}" << std::endl;
    os << "\\end{center}" << std::endl;
    delete leafified;
  }

 private:

  Ratio get_width() {
    return (SERIES != E12_SERIES) ? Ratio(1) : Ratio(3, 2);
  }

  // Helper function for writing schematics
  void output_coord(std::ostream& os, const Ratio& begin, const Ratio& end, const std::string& s = "") {
    os << "(" << boost::rational_cast<double>(begin) << "," << boost::rational_cast<double>(end) << ")";
    if (!s.empty()) os << s;
    else os << std::endl;
  }

  // First, calculate the area for each visual -- coords will all be zero.
  void calc_area(Node* node, Visual& visual, char op1 = '+', char op2 = '|') {
    if (node->children.empty()) {
      visual = Visual(0, 0, get_width(), 1, node->values.front());
      return;
    }
    for (auto child : node->children) {
      visual.subvisuals.push_back(Visual());
      calc_area(child, visual.subvisuals.back(), op2, op1);
    }
    for (unsigned v = 0; v < visual.subvisuals.size(); ++v) {
      Visual& subvisual = visual.subvisuals[v];
      // If we're drawing a parallel circuit, might need extra horizontal bar
      if (op1 == '+' && subvisual.v == INT_MAX && (!v || visual.subvisuals[v - 1].v == INT_MAX)) visual.w += ONE_QUARTER;
      visual.w = (op1 == '+') ? (visual.w + subvisual.w) : max(visual.w, subvisual.w);
      visual.h = (op1 == '|') ? (visual.h + subvisual.h) : max(visual.h, subvisual.h);
    }
    if (op1 == '+' && visual.subvisuals.back().v == INT_MAX) visual.w += ONE_QUARTER;
  }

  // Second, use all the areas to compute the coordinates of each visual.
  void update_coords(Visual& visual, const Ratio& _x = 0, const Ratio& _y = 0, char op1 = '+', char op2 = '|') {
    Ratio x = _x, y = _y;
    visual.x += x;
    visual.y += y;
    for (unsigned v = 0; v < visual.subvisuals.size(); ++v) {
      Visual& subvisual = visual.subvisuals[v];
      // If we're drawing a parallel circuit, might need extra horizontal bar
      if (op1 == '+' && subvisual.v == INT_MAX && (!v || visual.subvisuals[v - 1].v == INT_MAX)) x += ONE_QUARTER;
      // If we're about to draw a singleton, bump it up if we contain parallels
      Ratio y_adjust = (op1 == '+' && subvisual.v != INT_MAX && visual.h > 1) ? ONE_HALF : 0;
      // Need to center circuits being drawn in parallel
      Ratio x_adjust = (op1 == '+') ? 0 : (visual.w - subvisual.w) / 2;
      update_coords(subvisual, x + x_adjust, y + y_adjust, op2, op1);
      if (op1 == '+') x += subvisual.w;
      if (op1 == '|') y += subvisual.h;
    }
    // If we just drew a parallel circuit, need an extra bar going to the right
    if (op1 == '+' && visual.v == INT_MAX && visual.subvisuals.back().v != INT_MAX) x += ONE_QUARTER;
  }

  // Third, flip eveything vertically ... we want 0 to mean top, but LaTeX wants
  // it to mean bottom.
  void flip(Visual& visual, Ratio height) {
    visual.y = height - visual.y;
    for (auto& subvisual : visual.subvisuals) flip(subvisual, height);
  }

  // Fourth, use the given visual to compute the tikzpicture code.
  void visualize_schematic(std::ostream& os, Visual& visual, char op1 = '+', char op2 = '|') {
    if (visual.v != INT_MAX) {
      // Draw a simple resistor
      output_coord(os, (visual.x    ), (visual.y), " to ");
      auto v = boost::rational_cast<long long>(SERIES[visual.v] * 10);
      os << "[R,l=";
      os << v / 10;
      if (v % 10) os << "." << v % 10;
      os << "<\\ohm>,-] ";
      output_coord(os, (visual.x + get_width()), (visual.y));
      return;
    }
    if (op1 == '|') {
      // Left vertical line
      Visual& front = visual.subvisuals.front(), back = visual.subvisuals.back();
      Ratio bump_begin = (front.h > 1) ? ONE_HALF : 0;
      Ratio bump_end   = ( back.h > 1) ? ONE_HALF : 0;
      output_coord(os, (visual.x), (visual.y - bump_begin), " to ");
      output_coord(os, (visual.x), (visual.y - visual.h + 1 + bump_end));
    }
    for (unsigned v = 0; v < visual.subvisuals.size(); ++v) {
      Visual& subvisual = visual.subvisuals[v];
      // Extra horizontal bar *outside* the parallel circuit
      if (op1 == '+' && subvisual.v == INT_MAX && (!v || visual.subvisuals[v - 1].v == INT_MAX)) {
        output_coord(os, (subvisual.x - ONE_QUARTER), (subvisual.y - ONE_HALF), " to ");
        output_coord(os, (subvisual.x              ), (subvisual.y - ONE_HALF));
      }
      // Left horizontal bar *inside* the parallel circuit
      if (op1 == '|' && visual.w > subvisual.w) {
        Ratio adjust = (visual.w - subvisual.w) / 2;
        output_coord(os, (visual.x         ), (subvisual.y), " to ");
        output_coord(os, (visual.x + adjust), (subvisual.y));
      }
      visualize_schematic(os, subvisual, op2, op1);
      // Right horizontal bar *inside* the parallel circuit
      if (op1 == '|' && visual.w > subvisual.w) {
        Ratio adjust = (visual.w - subvisual.w) / 2;
        output_coord(os, (visual.x + visual.w - adjust), (subvisual.y), " to ");
        output_coord(os, (visual.x + visual.w         ), (subvisual.y));
      }
    }
    // Extra horizontal bar *outside* the parallel circuit
    if (op1 == '+' && visual.subvisuals.back().v == INT_MAX) {
      Visual& back = visual.subvisuals.back();
      output_coord(os, (back.x + back.w              ), (back.y - ONE_HALF), " to ");
      output_coord(os, (back.x + back.w + ONE_QUARTER), (back.y - ONE_HALF));
    }
    if (op1 == '|') {
      // Right vertical line
      Visual& front = visual.subvisuals.front(), back = visual.subvisuals.back();
      Ratio bump_begin = (front.h > 1) ? ONE_HALF : 0;
      Ratio bump_end   = ( back.h > 1) ? ONE_HALF : 0;
      output_coord(os, (visual.x + visual.w), (visual.y - bump_begin), " to ");
      output_coord(os, (visual.x + visual.w), (visual.y - visual.h + 1 + bump_end));
    }
  }

  void visualize_tree(std::ostream& os, Node* node, char op1, char op2) {
    os << "[";
    if (node->children.empty()) {
      if (node->values.size() > 1) os << "{";
      os << "$";
      for (auto v = node->values.begin(); v != node->values.end(); ++v) {
        if (v != node->values.begin()) os << ",";
        os << *v;
      }
      os << "$";
      if (node->values.size() > 1) os << "},dashed";
    } else {
      if (op1 == '|') os << ",circle";
      os << ",fill=" << (op1 == '+' ? "black" : "lightgray") << ",draw";
      for (auto child : node->children) visualize_tree(os, child, op2, op1);
    }
    os << "]";
  }
} visualizer;

}

#endif
