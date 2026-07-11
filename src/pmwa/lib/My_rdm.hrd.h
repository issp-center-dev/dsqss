#ifndef MY_RDM_H
#define MY_RDM_H

#include <fstream>
#include <iostream>
#include <random>
#include <string>

// Much of the pmwa code uses unqualified std names and has
// historically relied on this using-directive leaking from this
// header. Keep it until the sources are cleaned up.
using namespace std;

// Random number source for PMWA based on std::mt19937.
//
// This replaces a two-tap GFSR generator (X(t) = X(t-32) ^ X(t-521));
// that generator family is known to exhibit three-point correlations
// that have produced systematic errors in Monte Carlo studies.
// Results for a given seed differ from previous DSQSS versions.
class My_rdm {
  std::mt19937 mt;

 public:
  void outgen(std::string const& fname);
  void ingen(std::string const& fname);

  // Uniform real in [0,1) with 53-bit resolution.
  // Callers rely on the value being strictly less than 1.
  double rdm() {
    const unsigned long long hi = mt() >> 5;  // upper 27 bits
    const unsigned long long lo = mt() >> 6;  // lower 26 bits
    return (hi * 67108864.0 + lo) * (1.0 / 9007199254740992.0);  // 2^-53
  }

  explicit My_rdm(long seed) : mt(static_cast<unsigned int>(seed)) {}
};

#endif
