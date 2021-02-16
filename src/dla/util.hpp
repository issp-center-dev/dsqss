// DSQSS (Discrete Space Quantum Systems Solver)
// Copyright (C) 2018- The University of Tokyo
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SRC_DLA_UTIL_HPP_
#define SRC_DLA_UTIL_HPP_

#include <cstdlib>

#include <iostream>
#include <vector>

namespace util {
template <class T>
int min_index(const std::vector<T>& xs) {
  const int n = xs.size();
  int i = 0;
  T xmin = xs[0];
  for (int j = 1; j < n; ++j) {
    if (xs[j] < xmin) {
      i = j;
      xmin = xs[j];
    }
  }
  return i;
}
template <class T>
int max_index(const std::vector<T>& xs) {
  const int n = xs.size();
  int i = 0;
  T xmax = xs[0];
  for (int j = 1; j < n; ++j) {
    if (xs[j] > xmax) {
      i = j;
      xmax = xs[j];
    }
  }
  return i;
}

void ERROR(const char* msg) {
  std::cerr << "ERROR: " << msg << std::endl;
  std::exit(1);
}

}  // namespace util

#endif  // SRC_DLA_UTIL_HPP_
