#ifndef SRC_DLA_SPECIFIC_BOSON_MEASURE_SPECIFIC_IMPL_H_
#define SRC_DLA_SPECIFIC_BOSON_MEASURE_SPECIFIC_IMPL_H_

#include <vector>

namespace Specific {

std::vector<double> diagonal_operators(int NXMAX) {
  std::vector<double> ret(NXMAX);
  for (int i = 0; i < NXMAX; ++i) {
    ret[i] = i;
  }
  return ret;
}

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
  MZSA1,
  MZSA2,
  MZSB1,
  MZSB2,

  NH1,
  Wxy2,
  NACC,  // number of quantities measured at each MC step
};
static const char ANAME[][NACC] = {
    "sgn",   "nv1",   "eb1",   "eb2",   "le1",   "mzua1", "mzua2", "mzub1",
    "mzub2", "mzsa1", "mzsa2", "mzsb1", "mzsb2", "nh1",   "wxy2"};

// observable specifier
enum {
  SIGN,  // Sign of weight
  ANV,   // (site) Average of the Number of Vertices
  ENE,   // ENErgy per site
  SPE,   // SPEcific heat
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
  AMZS,
  BMZS,
  SMZS,
  XMZS,

  DS1,
  W2,    // Winding number^2
  RHOS,  // Superfluid density
  COMP,  // COMPressibility

  NPHY,  // number of quantities computed at each set
};
static const char PNAME[][NPHY] = {
    "sign", "anv",  "ene",  "spe",  "len",  "xmx", "amzu", "bmzu", "smzu",
    "xmzu", "amzs", "bmzs", "smzs", "xmzs", "ds1", "wi2",  "rhos", "comp"};

}  // namespace Specific

#endif  // SRC_DLA_SPECIFIC_BOSON_MEASURE_SPECIFIC_IMPL_H_
