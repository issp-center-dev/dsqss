#ifndef SF_H
#define SF_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include "debug.hpp"
#include "accumulator.hpp"
#include "parameter.hpp"
#include "wavevector.hpp"
#include "algorithm.hpp"
#include "lattice.hpp"

class SF {
private:
  bool to_be_calc;
  Lattice& LAT;
  Algorithm& ALG;
  WaveVector& wv;
  int NK;
  int Ntau;
  double dtau;

  Accumulator SIGN;
  std::vector<std::vector<Accumulator> > ACC;
  std::vector<std::vector<Accumulator> > PHY;

  std::vector<std::vector<double> > counterC;
  std::vector<std::vector<double> > counterS;

public:
  SF(Parameter const& param, Lattice& lat, Algorithm& alg, WaveVector& wv);
  void setinit();
  void measure(double sgn);
  void setsummary();
  void summary();
#ifdef MULTI
  void allreduce(MPI_Comm comm);
#endif
  void show(FILE*);

  void reset() {
    AutoDebugDump("SF::reset()");
    for (int ik = 0; ik < NK; ik++) {
      for (int it = 0; it < Ntau; it++) {
        counterC[ik][it] = 0.0;
        counterS[ik][it] = 0.0;
      }
    }
  };

  void save(std::ofstream& F) const;
  void load(std::ifstream& F);

  bool calculated() { return to_be_calc; }
};

SF::SF(Parameter const& param, Lattice& lat, Algorithm& alg, WaveVector& wv)
    : to_be_calc(false),
      LAT(lat),
      ALG(alg),
      wv(wv),
      NK(wv.NK),
      Ntau(param.NTAU),
      dtau(param.BETA/param.NTAU)
{
  AutoDebugDump("SF::SF");
  if (wv.defined){
    to_be_calc = true;
    SIGN.reset();
    for (int i = 0; i < NK; i++) {
      ACC.push_back(std::vector<Accumulator>(Ntau));
      PHY.push_back(std::vector<Accumulator>(Ntau));
      for (int it = 0; it < Ntau; it++) {
        std::stringstream ss;
        ss << "C" << i << "t" << it;
        ACC[i][it].reset();
        PHY[i][it].reset(ss.str());
      }
    }
    for (int ik = 0; ik < NK; ik++) {
      counterC.push_back(std::vector<double>(Ntau));
      counterS.push_back(std::vector<double>(Ntau));
    }
  }
};

inline void SF::setinit() {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::setinit");
  SIGN.reset();
  for (int i = 0; i < NK; i++)
    for (int itau = 0; itau < Ntau; itau++)
      ACC[i][itau].reset();
}

void SF::measure(double sgn) {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::measure");
  reset();

  for (int s = 0; s < LAT.NSITE; s++) {
    Site& SITE = LAT.S(s);
    SiteProperty& SP = SITE.Property();
    Site::iterator p(SITE);

    double bTri = 0.0;
    double tau  = 0.0;
    int it      = 0;

    while (!(++p).atOrigin()) {
      Segment& S = *p;
      double mz  = SP.XVALS[S.X()];

      double tTri = bTri + S.length();

      while (it < Ntau && tau < tTri) {
        for (int k = 0; k < NK; k++) {
          counterC[k][it] += mz * wv.COSrk[s][k];
          counterS[k][it] += mz * wv.SINrk[s][k];
        }

        it++;
        tau = it * dtau;
      }

      bTri = tTri;
    }
  }

  const double invNtau = 1.0 / Ntau;

  SIGN.accumulate(sgn);
  for (int ik = 0; ik < NK; ik++) {
    for (int it = 0; it < Ntau; it++) {
      double SZKT = 0.0;
      for (int tt = 0; tt < Ntau; tt++) {
        SZKT += counterC[ik][tt] * counterC[ik][(tt + it) % Ntau]
                + counterS[ik][tt] * counterS[ik][(tt + it) % Ntau];
      }
      ACC[ik][it].accumulate(sgn * SZKT * invNtau);
    }
  }
}

inline void SF::show(FILE* F) {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::show");
  for (int i = 0; i < NK; i++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[i][it].show(F,"R");
    }
    fprintf(F, "\n");
  }
};

inline void SF::summary() {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::summary");
  for (int i = 0; i < NK; i++)
    for (int itau = 0; itau < Ntau; itau++)
      PHY[i][itau].average();
}

void SF::setsummary() {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::setsummary");
  const double invV = 1.0 / LAT.NSITE;
  SIGN.average();
  const double invsign = 1.0/SIGN.mean();
  for (int ik = 0; ik < NK; ik++) {
    for (int it = 0; it < Ntau; it++) {
      ACC[ik][it].average();
      PHY[ik][it].accumulate(invsign * invV * ACC[ik][it].mean());
    }
  }
}

#ifdef MULTI
void SF::allreduce(MPI_Comm comm){
  for (int ik = 0; ik < NK; ik++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[ik][it].allreduce(comm);
    }
  }
}
#endif

void SF::save(std::ofstream& F) const {
  Serialize::save(F, SIGN);
  Serialize::save(F, ACC);
  Serialize::save(F, PHY);
}

void SF::load(std::ifstream& F) {
  Serialize::load(F, SIGN);
  Serialize::load(F, ACC);
  const int nk = ACC.size();
  if(nk != NK){
    std::cerr << "ERROR: NK is mismatched" << std::endl;
    std::cerr << "       sfinpfile maybe changed." << std::endl;
    exit(1);
  }
  if(NK > 0){
    const int ntau = ACC[0].size();
    if(ntau != Ntau){
      std::cerr << "ERROR: Ntau is mismatched" << std::endl;
      std::cerr << "       sfinpfile maybe changed." << std::endl;
      exit(1);
    }
  }
  Serialize::load(F, PHY);
}

#endif  // SF_H
