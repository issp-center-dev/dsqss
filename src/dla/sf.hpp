#ifndef SF_H
#define SF_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include "debug.hpp"
#include "accumulator.hpp"
#include "parameter.hpp"

class SF {
private:
  bool to_be_calc;
  Lattice& LAT;
  Algorithm& ALG;
  int Ntau;
  int NtauAll;
  int NKMAX;
  double dtau;

  int NSF;
  std::vector<double> dvals;

  Accumulator SIGN;
  std::vector<std::vector<double> > COSrk;
  std::vector<std::vector<double> > SINrk;
  std::vector<std::vector<Accumulator> > ACC;
  std::vector<std::vector<Accumulator> > PHY;

  std::vector<std::vector<double> > counterC;
  std::vector<std::vector<double> > counterS;

public:
  SF(Parameter const& param, Lattice& lat, Algorithm& alg);
  void read(XML::Block const& X);
  void setinit();
  void measure(double sgn);
  void setsummary();
  void summary();
#ifdef MULTI
  void allreduce(MPI_Comm comm);
#endif
  void show(FILE*);

  void reset() {
    for (int isf = 0; isf < NSF; isf++) {
      for (int it = 0; it < NtauAll; it++) {
        counterC[isf][it] = 0.0;
        counterS[isf][it] = 0.0;
      }
    }
  };

  void save(std::ofstream& F) const;
  void load(std::ifstream& F);

  bool calculated() { return to_be_calc; }
};

SF::SF(Parameter const& param, Lattice& lat, Algorithm& alg)
    : to_be_calc(false),
      LAT(lat),
      ALG(alg),
      Ntau(0),
      NtauAll(0),
      NKMAX(0),
      dtau(0.0),
      NSF(0),
      dvals(Specific::diagonal_operators(alg.NXMAX))
{
  AutoDebugDump("SF::SF");
  if (param.SFINPFILE.length() > 0) {
    to_be_calc = true;
    XML::Block X(param.SFINPFILE.c_str());
    NtauAll   = X["Ntau"].getInteger();
    int Nline = X["NumberOfElements"].getInteger();
    dtau = LAT.BETA / ((double)NtauAll);

    Ntau  = X["CutoffOfNtau"].getInteger();
    NKMAX = X["NumberOfInverseLattice"].getInteger();

    for (int i = 0; i < LAT.NSITE; i++) {
      COSrk.push_back(std::vector<double>(NKMAX));
      SINrk.push_back(std::vector<double>(NKMAX));
    }

    int count = 0;
    for (int i = 0; i < X.NumberOfBlocks(); i++) {
      XML::Block& B = X[i];
      if (B.getName() == "SF") {
        double COSrk_ = B.getDouble(0);
        double SINrk_ = B.getDouble(1);
        int isite     = B.getInteger(2);
        int ksite     = B.getInteger(3);
        //cout<<isite<<"-"<<jsite<<endl;
        COSrk[isite][ksite] = COSrk_;
        SINrk[isite][ksite] = SINrk_;
        count++;
      }
    }

    SIGN.reset();
    NSF = NKMAX;
    for (int i = 0; i < NSF; i++) {
      ACC.push_back(std::vector<Accumulator>(Ntau));
      PHY.push_back(std::vector<Accumulator>(Ntau));
      for (int it = 0; it < Ntau; it++) {
        std::stringstream ss;
        ss << "C" << i << "t" << it;
        ACC[i][it].reset();
        PHY[i][it].reset(ss.str());
      }
    }

    for (int isf = 0; isf < NSF + 1; isf++) {
      counterC.push_back(std::vector<double>(NtauAll));
      counterS.push_back(std::vector<double>(NtauAll));
    }

    if (count != Nline) {
      std::cout << "ERROR Nline( " << Nline << " ) != count( " << count << " )" << std::endl;
      exit(0);
    }
  }
};

inline void SF::setinit() {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::setinit");
  SIGN.reset();
  for (int i = 0; i < NSF; i++)
    for (int itau = 0; itau < Ntau; itau++)
      ACC[i][itau].reset();
}

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

      while (it < NtauAll && tau < tTri) {
        for (int k = 0; k < NKMAX; k++) {
          counterC[k][it] += mz * COSrk[s][k];
          counterS[k][it] += mz * SINrk[s][k];
        }

        it++;
        tau = it * dtau;
      }

      bTri = tTri;
    }
  }

  const double invNtauAll = 1.0 / NtauAll;

  SIGN.accumulate(sgn);
  for (int isf = 0; isf < NSF; isf++) {
    for (int it = 0; it < Ntau; it++) {
      double SZKT = 0.0;
      for (int tt = 0; tt < NtauAll; tt++) {
        SZKT += counterC[isf][tt] * counterC[isf][(tt + it) % NtauAll]
                + counterS[isf][tt] * counterS[isf][(tt + it) % NtauAll];
      }
      ACC[isf][it].accumulate(sgn * SZKT * invNtauAll);
    }
  }
}

inline void SF::show(FILE* F) {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::show");
  for (int i = 0; i < NSF; i++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[i][it].show(F,"R");
    }
    fprintf(F, "\n");
  }
};

inline void SF::summary() {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::summary");
  for (int i = 0; i < NSF; i++)
    for (int itau = 0; itau < Ntau; itau++)
      PHY[i][itau].average();
}

void SF::setsummary() {
  if (!to_be_calc) { return; }
  AutoDebugDump("SF::setsummary");
  const double invV = 1.0 / LAT.NSITE;
  SIGN.average();
  const double invsign = 1.0/SIGN.mean();
  for (int isf = 0; isf < NSF; isf++) {
    for (int it = 0; it < Ntau; it++) {
      ACC[isf][it].average();
      PHY[isf][it].accumulate(invsign * invV * ACC[isf][it].mean());
    }
  }
}

#ifdef MULTI
void SF::allreduce(MPI_Comm comm){
  for (int isf = 0; isf < NSF; isf++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[isf][it].allreduce(comm);
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
  const int nsf = ACC.size();
  if(nsf != NSF){
    std::cerr << "ERROR: NSF is mismatched" << std::endl;
    std::cerr << "       sfinpfile maybe changed." << std::endl;
    exit(1);
  }
  if(nsf > 0){
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
