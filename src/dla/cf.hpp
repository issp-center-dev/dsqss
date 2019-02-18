#ifndef CF_H
#define CF_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include "debug.hpp"
#include "accumulator_set.hpp"
#include "displacements.hpp"
#include "parameter.hpp"
#include "algorithm.hpp"
#include "lattice.hpp"

class CF {
private:
  bool to_be_calc;
  Lattice& LAT;
  Algorithm& ALG;
  Displacements dr;
  int nr;
  int ntau;
  double dtau;
  Accumulator_set accs;
  std::vector<double> dvals;

  std::vector<std::vector<double> > counter;

public:
  CF(Parameter const& param, Lattice& lat, Algorithm& alg);

  void measure(double sgn);
  void accumulate(int NCYC, double sgn);
  void count(double tT, double bT, int head_site, int tail_site, double tail_tau);

  void reset() {
    for (int ir = 0; ir < nr; ++ir) {
      for (int it = 0; it < ntau; ++it) {
        counter[ir][it] = 0.0;
      }
    }
  }
  bool calculated() { return to_be_calc; }

  // delegeted functions
  void setinit(){accs.setinit();}
  void summary(){accs.summary();}
  void setsummary(){accs.setsummary();}
#ifdef MULTI
  void allreduce(MPI_Comm comm){accs.allreduce(comm);}
#endif
  void show(FILE* F){accs.show(F);}
  void save(std::ofstream& F) const {accs.save(F);}
  void load(std::ifstream& F) { accs.load(F);}

};

CF::CF(Parameter const& param, Lattice& lat, Algorithm& alg)
    : to_be_calc(false),
      LAT(lat),
      ALG(alg),
      dr(param.CFINPFILE.c_str(), lat.NSITE),
      nr(dr.nr),
      ntau(param.NTAU),
      dtau(lat.BETA/ntau),
      accs(ntau, nr),
      dvals(Specific::diagonal_operators(alg.NXMAX))
{
  AutoDebugDump("CF::CF");
  if (param.CFINPFILE.length() > 0) {
    to_be_calc = true;

    for (int ir = 0; ir < nr + 1; ++ir) {
      counter.push_back(std::vector<double>(ntau));
    }
  }
};

void CF::count(double tT, double bT, int head_site, int tail_site, double tail_tau) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::count");

  int ir    = dr.r_id[tail_site][head_site];
  double bTr = tail_tau - tT;
  double tTr = tail_tau - bT;

  bTr += bTr < 0.0 ? LAT.BETA : 0.0;   // [0,B)
  tTr += tTr <= 0.0 ? LAT.BETA : 0.0;  // (0,B]

  int bTri = bTr/dtau + 1;
  int tTri = tTr/dtau;

  if (bTr <= tTr) {
    for (int ktau = bTri; ktau <= tTri; ktau++) {
      counter[ir][ktau%ntau] += 1;
    }
  } else {
    for (int ktau = 0; ktau <= tTri; ktau++) {
      counter[ir][ktau] += 1;
    }
    for (int ktau = bTri; ktau < ntau; ktau++) {
      counter[ir][ktau] += 1;
    }
  }

  return;
}

inline void CF::accumulate(int NCYC, double sgn) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::accumulate");
  const double invNCYC = 1.0 / NCYC;

  accs.accumulate_sign(sgn);
  std::vector<double> gt(ntau);
  for (int ir = 0; ir < nr; ir++){
    for (int it = 0; it < ntau; it++){
      gt[it] = sgn * invNCYC * counter[ir][it];
    }
    accs.accumulate(sgn, ir, gt);
  }
}
#endif  // CF_H
