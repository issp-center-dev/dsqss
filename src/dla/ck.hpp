#ifndef CK_H
#define CK_H

#include <cstring>
#include <fstream>
#include <sstream>
#include "debug.hpp"
#include "accumulator.hpp"
#include "parameter.hpp"

class CK {
private:
  bool to_be_calc;
  Lattice& LAT;
  Algorithm& ALG;
  int Ntau;
  int NtauAll;
  int NKMAX;
  double dtau;

  int NCK;

  std::vector<std::vector<double> > COSrk;
  std::vector<std::vector<double> > SINrk;
  std::vector<std::vector<Accumulator> > ACC;
  std::vector<std::vector<Accumulator> > PHY;

  std::vector<std::vector<double> >  counterC;

public:
  CK(Parameter const& param, Lattice& lat, Algorithm& alg);
  void read(const char* filename, int NSITE, double BETA);
  void setinit();
  void reset();
  void count(int s, double tT, double bT, double tail_tau);
  void accumulate(int NCYC);
  void setsummary();
  void summary();
#ifdef MULTI
  void allreduce(MPI_Comm comm);
#endif 
  void show(FILE*);

  void save(std::ofstream& F) const;
  void load(std::ifstream& F);

};

CK::CK(Parameter const& param, Lattice& lat, Algorithm& alg)
    : to_be_calc(false),
      LAT(lat),
      ALG(alg),
      Ntau(0),
      NtauAll(0),
      NKMAX(0),
      dtau(0.0),
      NCK(0)
{
  AutoDebugDump("CK::CK");
  if (param.CKINPFILE.length() > 0) {
    to_be_calc = true;
    XML::Block X(param.CKINPFILE.c_str());
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

    NCK = NKMAX;
    for (int i = 0; i < NCK; i++) {
      ACC.push_back(std::vector<Accumulator>(Ntau));
      PHY.push_back(std::vector<Accumulator>(Ntau));
      for (int it = 0; it < Ntau; it++) {
        std::stringstream ss;
        ss << "C" << i << "t" << it;
        ACC[i][it].reset();
        PHY[i][it].reset(ss.str());
      }
    }

    if (count != Nline) {
      std::cout << "ERROR Nline( " << Nline << " ) != count( " << count << " )" << std::endl;
      exit(0);
    }

    for (int ick = 0; ick < NCK + 1; ick++) {
      counterC.push_back(std::vector<double>(NtauAll));
    }
  }
};

inline void CK::setinit() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CK::setinit");
  for (int i = 0; i < NCK; i++)
    for (int itau = 0; itau < Ntau; itau++)
      ACC[i][itau].reset();
}

inline void CK::show(FILE* F) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CK::show");
  for (int i = 0; i < NCK; i++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[i][it].show(F,"R");
    }
    fprintf(F, "\n");
  }
};

inline void CK::accumulate(int NCYC) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CK::accumulate");
  const double invNCYC = 1.0 / NCYC;

  for (int ick = 0; ick < NCK; ick++)
    for (int it = 0; it < Ntau; it++)
      ACC[ick][it].accumulate(invNCYC * counterC[ick][it]);
};

inline void CK::summary() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CK::summary");
  for (int i = 0; i < NCK; i++)
    for (int itau = 0; itau < Ntau; itau++)
      PHY[i][itau].average();
}

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
    for (int ick = 0; ick < NKMAX; ick++) {
      for (int ktau = bTri; ktau <= tTri; ktau++) {
        counterC[ick][ktau%Ntau] += COSrk[s][ick];
      }
    }
  } else {
    for (int ick = 0; ick < NKMAX; ick++) {
      for (int ktau = 0; ktau <= tTri; ktau++) {
        counterC[ick][ktau] += COSrk[s][ick];
      }
      for (int ktau = bTri; ktau < Ntau; ktau++) {
        counterC[ick][ktau] += COSrk[s][ick];
      }
    }
  }
};

void CK::reset() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CK::reset");
  for (int ick = 0; ick < NCK; ick++) {
    for (int it = 0; it < NtauAll; it++) {
      counterC[ick][it] = 0.0;
      // counterS[ick][it] = 0.0;
    }
  }
};
void CK::setsummary() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CK::setsummary");
  const double factor = 2 * ALG.getBlock("WDIAG", (double)1.0);  //ALG.X["General"]["WDIAG" ].getDouble(); // 0.25
  for (int ick = 0; ick < NCK; ick++) {
    for (int it = 0; it < Ntau; it++) {
      ACC[ick][it].average();
      PHY[ick][it].accumulate(ACC[ick][it].mean() * factor);
    }
  }
}

#ifdef MULTI
void CK::allreduce(MPI_Comm comm) {
  for (int ick = 0; ick < NCK; ick++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[ick][it].allreduce(comm);
    }
  }
}
#endif

void CK::save(std::ofstream& F) const {
  Serialize::save(F, ACC);
  Serialize::save(F, PHY);
}

void CK::load(std::ifstream& F) {
  Serialize::load(F, ACC);
  const int nck = ACC.size();
  if(nck != NCK){
    std::cerr << "ERROR: NCK is mismatched" << std::endl;
    std::cerr << "       ckinpfile maybe changed." << std::endl;
    exit(1);
  }
  if(nck > 0){
    const int ntau = ACC[0].size();
    if(ntau != Ntau){
      std::cerr << "ERROR: Ntau is mismatched" << std::endl;
      std::cerr << "       ckinpfile maybe changed." << std::endl;
      exit(1);
    }
  }
  Serialize::load(F, PHY);
}

#endif  // CK_H
