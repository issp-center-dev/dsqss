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

#ifndef SRC_COMMON_FROM_STRING_H_
#define SRC_COMMON_FROM_STRING_H_

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <stdexcept>
#include <string>

#include "fpcheck.h"

// Strict string-to-number conversion replacing boost::lexical_cast:
// the whole string (after optional leading whitespace) must be
// consumed, otherwise std::runtime_error is thrown, so inputs like
// "12abc" are rejected instead of being silently truncated.
// "inf" is accepted for doubles (simulationtime = INF relies on it),
// but "nan" is rejected: a NaN sneaking in would silently pass every
// later comparison, and it cannot be detected downstream because
// std::isnan is unreliable under -ffast-math.

template <typename T>
T from_string(const std::string& s);

template <>
inline double from_string<double>(const std::string& s) {
  const char* begin = s.c_str();
  char* end = 0;
  errno = 0;
  double v = std::strtod(begin, &end);
  if (end == begin || *end != '\0') {
    throw std::runtime_error("from_string: not a number: \"" + s + "\"");
  }
  if (errno == ERANGE) {
    throw std::runtime_error("from_string: out of range: \"" + s + "\"");
  }
  if (dsqss::is_nan(v)) {
    throw std::runtime_error("from_string: nan is not allowed: \"" + s +
                             "\"");
  }
  return v;
}

template <>
inline int from_string<int>(const std::string& s) {
  const char* begin = s.c_str();
  char* end = 0;
  errno = 0;
  long v = std::strtol(begin, &end, 10);
  if (end == begin || *end != '\0') {
    throw std::runtime_error("from_string: not an integer: \"" + s + "\"");
  }
  if (errno == ERANGE || v < INT_MIN || v > INT_MAX) {
    throw std::runtime_error("from_string: out of range: \"" + s + "\"");
  }
  return static_cast<int>(v);
}

#endif  // SRC_COMMON_FROM_STRING_H_
