#ifndef CF_H
#define CF_H

#include <cstdio>
#include <fstream>
#include <sstream>
#include <cmath>
#include "debug.hpp"
#include "accumulator.hpp"
#include "parameter.hpp"

class CF {
private:
  bool to_be_calc;
  Lattice& LAT;
  Algorithm& ALG;
  int Ntau;
  int NKMAX;
  double dtau;

  int NCF;
  std::vector<int> DCF;
  std::vector<std::vector<int> > ICF;

  std::vector<std::vector<Accumulator> > ACC;
  std::vector<std::vector<Accumulator> > PHY;

  std::vector<std::vector<int> > counter;

public:
  CF(Parameter const& param, Lattice& lat, Algorithm& alg);
  void read(XML::Block const& X);
  void setinit();
  void reset();
  void count(double tT, double bT, int head_site, int tail_site, double tail_tau);
  void accumulate(int NCYC);
  void setsummary();
  void summary();
#ifdef MULTI
  void allreduce(MPI_Comm comm);
#endif 
  void show(FILE*);

  void save(std::ofstream& F) const; 
  void load(std::ifstream& F);

  bool calculated() { return to_be_calc; }
};

CF::CF(Parameter const& param, Lattice& lat, Algorithm& alg)
    : to_be_calc(false),
      LAT(lat),
      ALG(alg),
      Ntau(0),
      NKMAX(0),
      dtau(0.0),
      NCF(0)
{
  AutoDebugDump("CF::CF");
  if (param.CFINPFILE.length() > 0) {
    to_be_calc = true;
    XML::Block X(param.CFINPFILE.c_str());
    Ntau      = X["Ntau"].getInteger();
    int Nline = X["NumberOfElements"].getInteger();
#ifdef NORM
    dtau = 1.0 / Ntau;
#else
    dtau = LAT.BETA / Ntau;
#endif

    NCF = X["NumberOfKinds"].getInteger();
    for (int i = 0; i < NCF; i++) {
      ACC.push_back(std::vector<Accumulator>(Ntau));
      PHY.push_back(std::vector<Accumulator>(Ntau));
      for (int it = 0; it < Ntau; it++) {
        std::stringstream ss;
        ss << "C" << i << "t" << it;
        ACC[i][it].reset();
        PHY[i][it].reset(ss.str());
      }
    }

    DCF.resize(NCF,0);

    for (int i = 0; i < LAT.NSITE; i++) {
      ICF.push_back(std::vector<int>(LAT.NSITE, NCF));
    }

    int count = 0;
    for (int i = 0; i < X.NumberOfBlocks(); i++) {
      XML::Block& B = X[i];
      if (B.getName() == "CF") {
        int icf_  = B.getInteger(0);
        int isite = B.getInteger(1);
        int jsite = B.getInteger(2);
        //cout<<isite<<"-"<<jsite<<endl;
        ICF[isite][jsite] = icf_;
        DCF[icf_]++;
        count++;
      }
    }

    if (count != Nline) {
      std::cout << "ERROR Nline( " << Nline << " ) != count( " << count << " )" << std::endl;
      exit(0);
    }

    for (int icf = 0; icf < NCF + 1; icf++) {
      counter.push_back(std::vector<int>(Ntau));
    }
  }
};

inline void CF::setinit() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::setinit");
  for (int i = 0; i < NCF; i++)
    for (int itau = 0; itau < Ntau; itau++)
      ACC[i][itau].reset();
}

inline void CF::show(FILE* F) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::show");
  for (int i = 0; i < NCF; i++){
    for (int it = 0; it < Ntau; it++){
      PHY[i][it].show(F,"R");
    }
    std::fprintf(F,"\n");
  }

};

inline void CF::accumulate(int NCYC) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::accumulate");
  const double invNCYC = 1.0 / NCYC;
  for (int icf = 0; icf < NCF; icf++)
    for (int it = 0; it < Ntau; it++)
      ACC[icf][it].accumulate(invNCYC * counter[icf][it]);
};

inline void CF::summary() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::summary");
  for (int i = 0; i < NCF; i++)
    for (int itau = 0; itau < Ntau; itau++)
      PHY[i][itau].average();
}

void CF::count(double tT, double bT, int head_site, int tail_site, double tail_tau) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::count");

  int icf    = ICF[tail_site][head_site];
  double bTr = tail_tau - tT;
  double tTr = tail_tau - bT;

#ifdef NORM
  bTr += bTr < 0.0 ? 1.0 : 0.0;   // [0,1)
  tTr += tTr <= 0.0 ? 1.0 : 0.0;  // (0,1]
#else
  bTr += bTr < 0.0 ? LAT.BETA : 0.0;   // [0,B)
  tTr += tTr <= 0.0 ? LAT.BETA : 0.0;  // (0,B]
#endif

  int bTri = bTr/dtau + 1;
  int tTri = tTr/dtau;

  if (bTr <= tTr) {
    for (int ktau = bTri; ktau <= tTri; ktau++) {
      counter[icf][ktau%Ntau] += 1;
    }
  } else {
    for (int ktau = 0; ktau <= tTri; ktau++) {
      counter[icf][ktau] += 1;
    }
    for (int ktau = bTri; ktau < Ntau; ktau++) {
      counter[icf][ktau] += 1;
    }
  }

  return;
}

void CF::reset() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::reset");
  for (int icf = 0; icf < NCF; icf++) {
    for (int it = 0; it < Ntau; it++) {
      counter[icf][it] = 0;
    }
  }
}

void CF::setsummary() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::setsummary");
  const double V        = LAT.NSITE;
  const double testDIAG = ALG.getBlock("WDIAG", (double)1.0);  //ALG.X["General"]["WDIAG" ].getDouble(); // 0.25
  for (int icf = 0; icf < NCF; icf++) {
    const double factor = 2 * testDIAG * V / ((double)DCF[icf]);
    for (int it = 0; it < Ntau; it++) {
      ACC[icf][it].average();
      PHY[icf][it].accumulate(ACC[icf][it].mean() * factor);
    }
  }
}

#ifdef MULTI
void CF::allreduce(MPI_Comm comm) {
  for (int icf = 0; icf < NCF; icf++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[icf][it].allreduce(comm);
    }
  }
}
#endif

void CF::save(std::ofstream& F) const {
  Serialize::save(F, ACC);
  Serialize::save(F, PHY);
}

void CF::load(std::ifstream& F) {
  Serialize::load(F, ACC);
  const int ncf = ACC.size();
  if(ncf != NCF){
    std::cerr << "ERROR: NCF is mismatched" << std::endl;
    std::cerr << "       cfinpfile maybe changed." << std::endl;
    exit(1);
  }
  if(ncf > 0){
    const int ntau = ACC[0].size();
    if(ntau != Ntau){
      std::cerr << "ERROR: Ntau is mismatched" << std::endl;
      std::cerr << "       sfinpfile maybe changed." << std::endl;
      exit(1);
    }
  }
  Serialize::load(F, PHY);
}

#endif  //CF_H
