// DSQSS (Discrete Space Quantum Systems Solver)
// Copyright (C) 2018- The University of Tokyo
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef CF_H
#define CF_H

#include <cstdio>
#include <fstream>
#include <sstream>
#include <cmath>
#include "debug.hpp"
#include "accumulator.hpp"
#include "parameter.hpp"
#include "displacement.hpp"
#include "lattice.hpp"
#include "algorithm.hpp"

class CF {
private:
  bool to_be_calc;
  Lattice& LAT;
  Algorithm& ALG;
  Displacement& DISP;
  int nkinds;
  int Ntau;
  double dtau;

  Accumulator SIGN;
  std::vector<std::vector<Accumulator> > ACC;
  std::vector<std::vector<Accumulator> > PHY;

  std::vector<std::vector<int> > counter;

public:
  CF(Parameter const& param, Lattice& lat, Algorithm& alg, Displacement& disp);
  void setinit();
  void reset();
  void count(double tT, double bT, int head_site, int tail_site, double tail_tau);
  void accumulate(int NCYC, double sgn);
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

CF::CF(Parameter const& param, Lattice& lat, Algorithm& alg, Displacement& disp)
    : to_be_calc(false),
      LAT(lat),
      ALG(alg),
      DISP(disp),
      nkinds(disp.nkinds),
      Ntau(param.NTAU),
      dtau(param.BETA/param.NTAU)
{
  AutoDebugDump("CF::CF");
  if (disp.defined) {
    to_be_calc = true;

    SIGN.reset();
    for (int i = 0; i < nkinds; i++) {
      ACC.push_back(std::vector<Accumulator>(Ntau));
      PHY.push_back(std::vector<Accumulator>(Ntau));
      for (int it = 0; it < Ntau; it++) {
        std::stringstream ss;
        ss << "C" << i << "t" << it;
        ACC[i][it].reset();
        PHY[i][it].reset(ss.str());
      }
    }

    for (int i = 0; i < nkinds; i++) {
      counter.push_back(std::vector<int>(Ntau));
    }
  }
};

inline void CF::setinit() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::setinit");
  SIGN.reset();
  for (int i = 0; i < nkinds; i++)
    for (int itau = 0; itau < Ntau; itau++)
      ACC[i][itau].reset();
}

inline void CF::show(FILE* F) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::show");
  for (int i = 0; i < nkinds; i++){
    for (int it = 0; it < Ntau; it++){
      PHY[i][it].show(F,"R");
    }
    std::fprintf(F,"\n");
  }

};

inline void CF::accumulate(int NCYC, double sgn) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::accumulate");
  SIGN.accumulate(sgn);
  const double invNCYC = 1.0 / NCYC;
  for (int icf = 0; icf < nkinds; icf++)
    for (int it = 0; it < Ntau; it++)
      ACC[icf][it].accumulate(sgn * invNCYC * counter[icf][it]);
};

inline void CF::summary() {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::summary");
  for (int i = 0; i < nkinds; i++)
    for (int itau = 0; itau < Ntau; itau++)
      PHY[i][itau].average();
}

void CF::count(double tT, double bT, int head_site, int tail_site, double tail_tau) {
  if (!to_be_calc) { return; }
  AutoDebugDump("CF::count");

  int icf    = DISP.IR[tail_site][head_site];
  double bTr = tail_tau - tT;
  double tTr = tail_tau - bT;

  bTr += bTr < 0.0 ? LAT.BETA : 0.0;   // [0,B)
  tTr += tTr <= 0.0 ? LAT.BETA : 0.0;  // (0,B]

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
  for (int icf = 0; icf < nkinds; icf++) {
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
  SIGN.average();
  const double sgn = SIGN.mean();
  if(sgn != 0.0){
    const double invsign = 1.0 / sgn;
    for (int icf = 0; icf < nkinds; icf++) {
      const double factor = 2 * testDIAG * V / DISP.NR[icf];
      for (int it = 0; it < Ntau; it++) {
        ACC[icf][it].average();
        PHY[icf][it].accumulate(invsign * ACC[icf][it].mean() * factor);
      }
    }
  }
}

#ifdef MULTI
void CF::allreduce(MPI_Comm comm) {
  for (int icf = 0; icf < nkinds; icf++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[icf][it].allreduce(comm);
    }
  }
}
#endif

void CF::save(std::ofstream& F) const {
  Serialize::save(F, SIGN);
  Serialize::save(F, ACC);
  Serialize::save(F, PHY);
}

void CF::load(std::ifstream& F) {
  Serialize::load(F, SIGN);
  Serialize::load(F, ACC);
  const int NKINDS = ACC.size();
  if(nkinds != NKINDS){
    std::cerr << "ERROR: nkinds is mismatched" << std::endl;
    std::cerr << "       cfinpfile maybe changed." << std::endl;
    exit(1);
  }
  if(nkinds > 0){
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
