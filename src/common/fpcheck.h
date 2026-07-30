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

#ifndef SRC_COMMON_FPCHECK_H_
#define SRC_COMMON_FPCHECK_H_

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

// Floating-point classification via integer bit operations.
// std::isfinite / std::isnan may be constant-folded away under
// -ffast-math (and the default fp-model of the Intel compilers),
// while the bit tests keep working under any compiler flags.
// The bit layout assumes IEEE-754 binary64; on exotic platforms
// where double is something else, fall back to the std functions
// (the fast-math concern does not arise for those FP formats anyway).

namespace dsqss {

inline bool double_is_ieee754() {
  return std::numeric_limits<double>::is_iec559 &&
         sizeof(double) == sizeof(std::uint64_t);
}

inline std::uint64_t double_bits(double v) {
  std::uint64_t u;
  std::memcpy(&u, &v, sizeof(u));
  return u;
}

// true iff v is neither infinite nor NaN
inline bool is_finite(double v) {
  if (double_is_ieee754()) {
    return ((double_bits(v) >> 52) & 0x7ffULL) != 0x7ffULL;
  }
  return std::isfinite(v);
}

inline bool is_nan(double v) {
  if (double_is_ieee754()) {
    const std::uint64_t u = double_bits(v);
    return ((u >> 52) & 0x7ffULL) == 0x7ffULL && (u & 0xfffffffffffffULL) != 0;
  }
  return std::isnan(v);
}

}  // namespace dsqss

#endif  // SRC_COMMON_FPCHECK_H_
