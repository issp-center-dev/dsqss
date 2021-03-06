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

#ifndef SRC_DLA_CK_HPP_
#define SRC_DLA_CK_HPP_

#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

#include "accumulator.hpp"
#include "algorithm.hpp"
#include "debug.hpp"
#include "lattice.hpp"
#include "parameter.hpp"
#include "wavevector.hpp"

class CK {
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

 public:
  CK(Parameter const& param, Lattice& lat, Algorithm& alg, WaveVector& wv);
  void read(const char* filename, int NSITE, double BETA);
  void setinit();
  void reset();
  void count(int s, double tT, double bT, double tail_tau);
  void accumulate(int NCYC, double sgn);
  void setsummary();
  void summary();
#ifdef MULTI
  void allreduce(MPI_Comm comm);
#endif
  void show(FILE*);

  void save(std::ofstream& F) const;
  void load(std::ifstream& F);
};

CK::CK(Parameter const& param, Lattice& lat, Algorithm& alg, WaveVector& wv)
    : to_be_calc(false),
      LAT(lat),
      ALG(alg),
      wv(wv),
      NK(wv.NK),
      Ntau(param.NTAU),
      dtau(param.BETA / Ntau) {
  AutoDebugDump("CK::CK");
  if (wv.defined) {
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

    for (int ik = 0; ik < NK + 1; ik++) {
      counterC.push_back(std::vector<double>(Ntau));
    }
  }
}

inline void CK::setinit() {
  if (!to_be_calc) {
    return;
  }
  AutoDebugDump("CK::setinit");
  SIGN.reset();
  for (int i = 0; i < NK; i++)
    for (int itau = 0; itau < Ntau; itau++) ACC[i][itau].reset();
}

inline void CK::show(FILE* F) {
  if (!to_be_calc) {
    return;
  }
  AutoDebugDump("CK::show");
  for (int i = 0; i < NK; i++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[i][it].show(F, "R");
    }
    fprintf(F, "\n");
  }
}

inline void CK::accumulate(int NCYC, double sgn) {
  if (!to_be_calc) {
    return;
  }
  AutoDebugDump("CK::accumulate");
  const double invNCYC = 1.0 / NCYC;

  SIGN.accumulate(sgn);
  for (int ik = 0; ik < NK; ik++)
    for (int it = 0; it < Ntau; it++)
      ACC[ik][it].accumulate(sgn * invNCYC * counterC[ik][it]);
}

inline void CK::summary() {
  if (!to_be_calc) {
    return;
  }
  AutoDebugDump("CK::summary");
  for (int i = 0; i < NK; i++)
    for (int itau = 0; itau < Ntau; itau++) PHY[i][itau].average();
}

void CK::count(int s, double tT, double bT, double tail_tau) {
  if (!to_be_calc) {
    return;
  }
  AutoDebugDump("CK::count");

  double bTr;
  double tTr;

  bTr = tail_tau - tT;
  tTr = tail_tau - bT;
  bTr += bTr < 0.0 ? LAT.BETA : 0.0;   // [0,BETA)
  tTr += tTr <= 0.0 ? LAT.BETA : 0.0;  // (0,BETA]
  int bTri = bTr / dtau + 1;
  int tTri = tTr / dtau;  // Relative bottom (top) time integer

  if (bTr <= tTr) {
    for (int ik = 0; ik < NK; ik++) {
      for (int ktau = bTri; ktau <= tTri; ktau++) {
        counterC[ik][ktau % Ntau] += wv.COSrk[s][ik];
      }
    }
  } else {
    for (int ik = 0; ik < NK; ik++) {
      for (int ktau = 0; ktau <= tTri; ktau++) {
        counterC[ik][ktau] += wv.COSrk[s][ik];
      }
      for (int ktau = bTri; ktau < Ntau; ktau++) {
        counterC[ik][ktau] += wv.COSrk[s][ik];
      }
    }
  }
}

void CK::reset() {
  if (!to_be_calc) {
    return;
  }
  AutoDebugDump("CK::reset");
  for (int ik = 0; ik < NK; ik++) {
    for (int it = 0; it < Ntau; it++) {
      counterC[ik][it] = 0.0;
      // counterS[ik][it] = 0.0;
    }
  }
}
void CK::setsummary() {
  if (!to_be_calc) {
    return;
  }
  AutoDebugDump("CK::setsummary");
  const double factor =
      2 * ALG.getBlock(
              "WDIAG",
              1.0);  // ALG.X["General"]["WDIAG" ].getDouble(); // 0.25
  SIGN.average();
  const double sgn = SIGN.mean();
  if (sgn != 0.0) {
    const double invsign = 1.0 / sgn;
    for (int ik = 0; ik < NK; ik++) {
      for (int it = 0; it < Ntau; it++) {
        ACC[ik][it].average();
        PHY[ik][it].accumulate(invsign * ACC[ik][it].mean() * factor);
      }
    }
  }
}

#ifdef MULTI
void CK::allreduce(MPI_Comm comm) {
  for (int ik = 0; ik < NK; ik++) {
    for (int it = 0; it < Ntau; it++) {
      PHY[ik][it].allreduce(comm);
    }
  }
}
#endif

void CK::save(std::ofstream& F) const {
  Serialize::save(F, SIGN);
  Serialize::save(F, ACC);
  Serialize::save(F, PHY);
}

void CK::load(std::ifstream& F) {
  Serialize::load(F, SIGN);
  Serialize::load(F, ACC);
  const int nk = ACC.size();
  if (nk != NK) {
    std::cerr << "ERROR: NK is mismatched" << std::endl;
    std::cerr << "       ckinpfile maybe changed." << std::endl;
    exit(1);
  }
  if (nk > 0) {
    const int ntau = ACC[0].size();
    if (ntau != Ntau) {
      std::cerr << "ERROR: Ntau is mismatched" << std::endl;
      std::cerr << "       ckinpfile maybe changed." << std::endl;
      exit(1);
    }
  }
  Serialize::load(F, PHY);
}

#endif  // SRC_DLA_CK_HPP_
