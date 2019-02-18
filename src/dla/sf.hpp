#ifndef SF_H
#define SF_H

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

class SF {
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
  std::vector<std::vector<double> > counterS;


public:
  SF(Parameter const& param, Lattice& lat, Algorithm& alg);

  void measure(double sgn);

  void reset() {
    for (int ik = 0; ik < nk; ++ik) {
      for (int it = 0; it < ntau; ++it) {
        counterC[ik][it] = 0.0;
        counterS[ik][it] = 0.0;
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

SF::SF(Parameter const& param, Lattice& lat, Algorithm& alg)
    : to_be_calc(false),
      LAT(lat),
      ALG(alg),
      wv(param.SFINPFILE.c_str()),
      nk(wv.nk),
      ntau(param.NTAU),
      dtau(lat.BETA/ntau),
      accs(ntau, nk),
      dvals(Specific::diagonal_operators(alg.NXMAX))
{
  AutoDebugDump("SF::SF");
  if (param.SFINPFILE.length() > 0) {
    to_be_calc = true;

    for (int ik = 0; ik < nk + 1; ++ik) {
      counterC.push_back(std::vector<double>(ntau));
      counterS.push_back(std::vector<double>(ntau));
    }
  }
};

void SF::measure(double sgn) {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::measure");
  using namespace Specific;
  reset();

  for (int s = 0; s < LAT.NSITE; s++) {
    Site& SITE = LAT.S(s);
    Site::iterator p(SITE);

    double bTri = 0.0;
    double tau  = 0.0;
    int it      = 0;

    while (!(++p).atOrigin()) {
      Segment& S = *p;
      double mz  = dvals[S.X()];

      double tTri = bTri + S.length();

      while (it < ntau && tau < tTri) {
        for (int k = 0; k < nk; k++) {
          counterC[k][it] += mz * wv.COSrk[s][k];
          counterS[k][it] += mz * wv.SINrk[s][k];
        }

        it++;
        tau = it * dtau;
      }

      bTri = tTri;
    }
  }

  const double coeff = 1.0 / (ntau*LAT.NSITE);

  accs.accumulate_sign(sgn);
  std::vector<double> SZKT(ntau);
  for (int ik = 0; ik < nk; ++ik) {
    for (int it = 0; it < ntau; ++it) {
      SZKT[it] = 0.0;
      for (int tt = 0; tt < ntau; ++tt) {
        SZKT[it] += coeff*(counterC[ik][tt] * counterC[ik][(tt + it) % ntau]
                         + counterS[ik][tt] * counterS[ik][(tt + it) % ntau]);
      }
    }
    accs.accumulate(sgn, ik, SZKT);
  }
}

#endif  // SF_H
