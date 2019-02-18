#ifndef CK_H
#define CK_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include "debug.hpp"
#include "accumulator_set.hpp"
#include "wavevector.hpp"
#include "parameter.hpp"
#include "algorithm.hpp"
#include "lattice.hpp"

class CK {
private:
  bool to_be_calc;
  Lattice& LAT;
  Algorithm& ALG;
  Wavevector wv;
  int nk;
  int ntau;
  double dtau;
  Accumulator_set accs;
  std::vector<double> dvals;

  std::vector<std::vector<double> > counterC;

public:
  CK(Parameter const& param, Lattice& lat, Algorithm& alg);

  void measure(double sgn);
  void accumulate(int NCYC, double sgn);
  void count(int s, double tT, double bT, double tail_tau);

  void reset() {
    for (int ik = 0; ik < nk; ++ik) {
      for (int it = 0; it < ntau; ++it) {
        counterC[ik][it] = 0.0;
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

CK::CK(Parameter const& param, Lattice& lat, Algorithm& alg)
    : to_be_calc(false),
      LAT(lat),
      ALG(alg),
      wv(param.CKINPFILE.c_str()),
      nk(wv.nk),
      ntau(param.NTAU),
      dtau(lat.BETA/ntau),
      accs(ntau, nk),
      dvals(Specific::diagonal_operators(alg.NXMAX))
{
  AutoDebugDump("CK::CK");
  if (param.CKINPFILE.length() > 0) {
    to_be_calc = true;

    for (int ik = 0; ik < nk + 1; ++ik) {
      counterC.push_back(std::vector<double>(ntau));
    }
  }
};

void CK::count(int s, double tT, double bT, double tail_tau) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CK::count");

  double bTr;
  double tTr;

  bTr = tail_tau - tT;
  tTr = tail_tau - bT;
  bTr += bTr < 0.0 ? LAT.BETA : 0.0;  // [0,BETA)
  tTr += tTr <= 0.0 ? LAT.BETA : 0.0;  // (0,BETA]
  int bTri = bTr / dtau + 1;
  int tTri = tTr / dtau;  //Relative bottom (top) time integer

  if (bTr <= tTr) {
    for (int ik = 0; ik < nk; ik++) {
      for (int ktau = bTri; ktau <= tTri; ktau++) {
        counterC[ik][ktau%ntau] += wv.COSrk[s][ik];
      }
    }
  } else {
    for (int ik = 0; ik < nk; ik++) {
      for (int ktau = 0; ktau <= tTri; ktau++) {
        counterC[ik][ktau] += wv.COSrk[s][ik];
      }
      for (int ktau = bTri; ktau < ntau; ktau++) {
        counterC[ik][ktau] += wv.COSrk[s][ik];
      }
    }
  }
}

inline void CK::accumulate(int NCYC, double sgn) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CK::accumulate");
  const double invNCYC = 1.0 / NCYC;

  accs.accumulate_sign(sgn);
  std::vector<double> gt(ntau);
  for (int ik = 0; ik < nk; ik++){
    for (int it = 0; it < ntau; it++){
      gt[it] = sgn * invNCYC * counterC[ik][it];
    }
    accs.accumulate(sgn, ik, gt);
  }
}

#endif  // CK_H
