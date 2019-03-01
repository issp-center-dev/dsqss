#ifndef MEASURE_H
#define MEASURE_H

#include <cstdio>
#include <vector>
#include "name.h"
#include "accumulator.hpp"
#include "parameter.hpp"
#include "measure_specific.h"

class Measurement {
private:
  int NACC;
  int NPHY;
  Parameter& P;
  Lattice& LAT;
  Algorithm& ALG;
  std::vector<double> Q;
  std::vector<double> dvals;

public:
  std::vector<Accumulator> ACC;  // accumurator of snapshot values
  std::vector<Accumulator> PHY;  // accumurator of set averages

  double ediag;
  int nkink;

  double EBASE;

  Measurement(Parameter& P0, Lattice& L, Algorithm& A);
  void measure(double sgn);
  void summary();
  void setinit();

  void setsummary();

#ifdef MULTI
  void allreduce(MPI_Comm comm);
#endif

  void accumulate_length(double len, double sgn);
  void dump();
  void show(FILE*);

  void save(std::ofstream& F) const;
  void load(std::ifstream& F);
};

inline Measurement::Measurement(Parameter& P0, Lattice& L, Algorithm& A)
    : NACC(Specific::NACC),
      NPHY(Specific::NPHY),
      P(P0),
      LAT(L),
      ALG(A),
      Q(NPHY),
      dvals(Specific::diagonal_operators(ALG.NXMAX)),
      ACC(NACC),
      PHY(NPHY) {
  AutoDebugDump("Measurement::Measurement");

  for (int i = 0; i < NACC; i++) {
    ACC[i].reset();
    ACC[i].set_key(Specific::ANAME[i]);
  }

  for (int i = 0; i < NPHY; i++) {
    PHY[i].reset();
    PHY[i].set_key(Specific::PNAME[i]);
  }

  // setting EBASE

  EBASE = 0.0;
  for (int i = 0; i < LAT.NINT; i++) {
    InteractionProperty& IP = LAT.I(i).property();
    double eb               = IP.EBASE;
    EBASE += eb;
  }
}

inline void Measurement::setinit() {
  for (int i = 0; i < NACC; i++)
    ACC[i].reset();
}

inline void Measurement::summary() {
  for (int i = 0; i < NPHY; i++) {
    PHY[i].average();
  }
}

#ifdef MULTI
inline void Measurement::allreduce(MPI_Comm comm){
  for (int i=0; i<NPHY; i++){
    PHY[i].allreduce(comm);
  }
}
#endif

inline void Measurement::show(FILE* F) {
  for (int i = 0; i < NPHY; i++) {
    PHY[i].show(F, "R");
  }
}

inline void Measurement::dump() {
  printf("\n");
  printf(">>> Dumping accumurators :\n");
  for (int i = 0; i < NACC; i++) {
    printf("ACC[%2d] : ", i);
    ACC[i].show();
  }
  printf("\n");
  for (int i = 0; i <= NPHY; i++) {
    printf("phy[%2d] : ", i);
    PHY[i].show();
  }
}

inline void Measurement::accumulate_length(double len, double sgn) { ACC[Specific::LE1].accumulate(len * sgn); }

void Measurement::save(std::ofstream& F) const {
  Serialize::save(F, ACC);
  Serialize::save(F, PHY);
}

void Measurement::load(std::ifstream& F) {
  Serialize::load(F, ACC);
  const int NACC_ = ACC.size();
  if (NACC_ != Specific::NACC) {
    std::cerr << "ERROR: NACC is mismatched" << std::endl;
    std::cerr << "       model maybe changed." << std::endl;
    exit(1);
  }

  Serialize::load(F, PHY);
  const int NPHY_ = PHY.size();
  if (NPHY_ != Specific::NPHY) {
    std::cerr << "ERROR: NPHY is mismatched" << std::endl;
    std::cerr << "       model maybe changed." << std::endl;
    exit(1);
  }
}

#include "measure_specific.cc"

#endif
