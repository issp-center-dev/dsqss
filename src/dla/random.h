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

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <cmath>
#include <cstdint>
#include <random>
#include <sstream>
#include <string>

#include "serialize.hpp"

typedef unsigned int Rint;

// Random number generator based on std::mt19937 (Mersenne Twister).
//
// This replaces a two-tap GFSR generator, X(t) = X(t-32) ^ X(t-521),
// whose generator family is known to exhibit three-point correlations
// that have produced measurable systematic errors in Monte Carlo
// studies (Ferrenberg, Landau, and Wong, PRL 69, 3382 (1992)).
// Simulation results obtained with a given seed differ from those of
// previous DSQSS versions; statistical properties are unchanged or
// improved.
class Random {
 private:
  std::mt19937 mt;

 public:
  explicit Random(Rint irand0 = 20000101, Rint /*nrbit0*/ = 32)
      : mt(irand0) {}

  void setSeed(Rint irand0, Rint /*nrbit0*/ = 32) { mt.seed(irand0); }

  // Uniform real in [0,1) with 53-bit resolution
  double Uniform() {
    const std::uint64_t hi = mt() >> 5;  // upper 27 bits
    const std::uint64_t lo = mt() >> 6;  // lower 26 bits
    return (hi * 67108864.0 + lo) * (1.0 / 9007199254740992.0);  // 2^-53
  }

  // Uniform integer in {0, 1, ..., ilimit-1}
  Rint Int(Rint ilimit) { return static_cast<Rint>(ilimit * Uniform()); }

  // Uniform 32-bit integer
  Rint Int() { return mt(); }

  double Exp() { return -std::log(1e0 - Uniform()); }

  double Gauss() {
    const double theta = 6.283185307179586477 * Uniform();
    return std::sqrt(-2e0 * std::log(1e0 - Uniform())) * std::sin(theta);
  }

  std::mt19937& engine() { return mt; }
  const std::mt19937& engine() const { return mt; }
};

namespace Serialize {

// Checkpoint the engine state as its standard textual representation
// instead of dumping raw object bytes, which is unspecified for
// std::mt19937 and not portable between standard library
// implementations.
inline void save(std::ofstream& ofs, const Random& rng) {
  std::ostringstream oss;
  oss << rng.engine();
  const std::string state = oss.str();
  save(ofs, state);
}

inline void load(std::ifstream& ifs, Random& rng) {
  std::string state;
  load(ifs, state);
  std::istringstream iss(state);
  iss >> rng.engine();
  if (!iss) {
    std::fprintf(stderr,
                 "ERROR: failed to restore the random number generator "
                 "state from the checkpoint file.\n");
    std::exit(1);
  }
}

}  // namespace Serialize

#endif  // _RANDOM_H_
