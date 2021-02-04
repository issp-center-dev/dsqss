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

#if 0
#if defined(MEASURE_BOSON) && !defined(MEASURE_SPIN) && \
    !defined(MEASURE_USER_SPECIFIC)
#include "SPECIFIC/Boson/measure_specific.h"
#elif !defined(MEASURE_BOSON) && defined(MEASURE_SPIN) && \
    !defined(MEASURE_USER_SPECIFIC)
#include "SPECIFIC/Heisenberg/measure_specific.h"
#elif !defined(MEASURE_BOSON) && !defined(MEASURE_SPIN) && \
    defined(MEASURE_USER_SPECIFIC)
#include "SPECIFIC/User/measure_specific.h"
#else
#error You should define ONE of the following tokens: MEASURE_BOSON, MEASURE_SPIN, or MEASURE_USER_SPECIFIC.
#endif
#endif

#include <vector>

namespace Specific {

// accumulator specifier
enum {
  SGN,  // Sign of weight
  NV1,  // Number of Vertices
  EB1,  // Number of kinks
  EB2,  // Number of kinks^2
  LE1,  // LEngth of worm pair

  // observables about Magnetization Z per site
  //  A: tau=0    B: tau-integrated
  //  U: Uniform  S: Staggered
  //  1: linear   2: squared
  MZUA1,
  MZUA2,
  MZUB1,
  MZUB2,

  NH1,
  Wxy2,
  NACC,  // number of quantities measured at each MC step
};
static std::string ANAME[NACC] = {"sgn",   "nv1",   "eb1",   "eb2",
                                  "le1",   "mzua1", "mzua2", "mzub1",
                                  "mzub2", "nh1",   "wxy2"};

// observable specifier
enum {
  SIGN,  // Sign of weight
  ANV,   // (site) Average of the Number of Vertices
  ENE,   // ENErgy per site
  SPE,   // SPEcific heat
  SOM,   // SOMmerfeld coeff
  LEN,   // LENgth of worm pair
  XMX,

  // observables about Magnetization Z per site
  // prefix:
  //  A: tau=0    B: tau-integrated
  //  S: Variance X: Susceptibility
  // postfix:
  //  U: Uniform, S: Staggered
  AMZU,
  BMZU,
  SMZU,
  XMZU,

  DS1,
  W2,    // Winding number^2
  RHOS,  // Superfluid density
  RHOF,  // Superfluid fraction
  COMP,  // COMPressibility

  NPHY,  // number of quantities computed at each set
};
static std::string PNAME[NPHY] = {"sign", "anv",  "ene",  "spe",  "som",  "len",
                                  "xmx",  "amzu", "bmzu", "smzu", "xmzu", "ds1",
                                  "wi2",  "rhos", "rhof", "comp"};

}  // namespace Specific
