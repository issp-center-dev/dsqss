#if 0
#if defined(MEASURE_BOSON) && !defined(MEASURE_SPIN) && !defined(MEASURE_USER_SPECIFIC)
#include "SPECIFIC/Boson/measure_specific.h"
#elif !defined(MEASURE_BOSON) && defined(MEASURE_SPIN) && !defined(MEASURE_USER_SPECIFIC)
#include "SPECIFIC/Heisenberg/measure_specific.h"
#elif !defined(MEASURE_BOSON) && !defined(MEASURE_SPIN) && defined(MEASURE_USER_SPECIFIC)
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
static std::string ANAME[NACC] = {"sgn", "nv1",   "eb1",   "eb2",   "le1",   "mzua1", "mzua2", "mzub1", "mzub2",
                                  "nh1",   "wxy2"};

// observable specifier
enum {
  SIGN, // Sign of weight
  ANV,  // (site) Average of the Number of Vertices
  ENE,  // ENErgy per site
  SPE,  // SPEcific heat
  LEN,  // LENgth of worm pair
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
static std::string PNAME[NPHY] = {"sign", "anv",  "ene",  "spe",  "len",  "xmx", "amzu", "bmzu", "smzu", "xmzu",
                                  "ds1", "wi2",  "rhos", "rhof", "comp"};

}  // namespace Specific
