#ifndef MEASURE_H
#define MEASURE_H

#include <cstdio>
#include <vector>
#include "name.h"
#include "accumulator.hpp"
#include "parameter.hpp"
#include "measure_specific.h"
#include "algorithm.hpp"
#include "lattice.hpp"

class Measurement {
private:
  int NACC;
  int NPHY;
  Parameter& P;
  Lattice& LAT;
  Algorithm& ALG;
  std::vector<double> Q;

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

void Measurement::measure(double sgn) {
  using namespace Specific;
  int NV = LAT.countVertices();

  double MZUA = 0.0;  // uniform,   tau=0
  double MZUB = 0.0;  // uniform,   integrated
  double MZSA = 0.0;  // staggered, tau=0
  double MZSB = 0.0;  // staggered, integrated

  const double T    = 1.0 / LAT.BETA;
  const double invV = 1.0 / LAT.NSITE;

  std::vector<double> phase(LAT.NSMTYPE);
  for(int m=0; m<LAT.NSMTYPE; ++m){
    phase[m] = std::cos(2*M_PI*m/LAT.NSMTYPE);
  }

  for (int s = 0; s < LAT.NSITE; s++) {
    Site& SITE  = LAT.S(s);
    SiteProperty& SP = SITE.Property();
    int mt      = SITE.getMTYPE();
    double ph   = phase[mt];
    Segment& S0 = SITE.first();
    double mz0  = SP.XVALS[S0.X()];
    Site::iterator p(SITE);
    double mza0 = 0.0;

    while (!(++p).atOrigin()) {
      Segment& S = *p;
      double mz  = SP.XVALS[S.X()];
      mza0 += mz * S.length();
    }

    MZUA += mz0;
    MZUB += mza0;
    MZSA += ph * mz0;
    MZSB += ph * mza0;
  }
  MZUA *= invV;
  MZSA *= invV;
  MZUB *= invV;
  MZSB *= invV;
  MZUB *= T;
  MZSB *= T;

  double EBSAMP = -(double)NV;

  for (int b = 0; b < LAT.NINT; b++) {
    Interaction& I          = LAT.I(b);
    InteractionProperty& IP = I.property();
    //VertexProperty& VP = IP.getVertexProperty();
    int NBODY = IP.NBODY;
    std::vector<double> tau(NBODY);
    std::vector<int> x(NBODY);
    std::vector<Site::iterator> p(NBODY);

    for (int i = 0; i < NBODY; i++) {
      Site& S = I.site(i);
      p[i].init(S);
      ++p[i];
      tau[i] = p[i]->topTime();
      x[i]   = p[i]->X();
    }

    double t = 0.0;

    while (t < LAT.BETA) {
      int it = util::min_index(tau);
      EBSAMP -= (tau[it] - t) * IP.VertexDensity(x);

      if (p[it]->top().isTerminal()) break;
      t = tau[it];
      ++p[it];
      tau[it] = p[it]->topTime();
      x[it]   = p[it]->X();
    }
  }

  ACC[SGN].accumulate(sgn);

  ACC[NV1].accumulate(sgn*NV);

  ACC[MZUA1].accumulate(sgn * MZUA);
  ACC[MZUA2].accumulate(sgn * MZUA * MZUA);
  ACC[MZUB1].accumulate(sgn * MZUB);
  ACC[MZUB2].accumulate(sgn * MZUB * MZUB);
  ACC[MZSA1].accumulate(sgn * MZSA);
  ACC[MZSA2].accumulate(sgn * MZSA * MZSA);
  ACC[MZSB1].accumulate(sgn * MZSB);
  ACC[MZSB2].accumulate(sgn * MZSB * MZSB);

  ACC[EB1].accumulate(sgn * EBSAMP);
  ACC[EB2].accumulate(sgn * EBSAMP * EBSAMP);
  ACC[NH1].accumulate(sgn * MZUA * EBSAMP);

  // WINDING NUMBER
  const int Dim = LAT.BD;
  if (Dim > 0) {
    std::vector<double> wind(Dim);

    for (int xi = 0; xi < LAT.NEDGE; xi++) {
      int Asite = LAT.EDGE(xi).A;
      int Bsite = LAT.EDGE(xi).B;
      int dim   = LAT.EDGE(xi).bd;

      Site& SITE  = LAT.S(Asite);
      Segment& S0 = SITE.first();
      Site::iterator p(SITE);
      int xlast = S0.X();

      //count winding

      while (!(++p).atOrigin()) {
        Segment& S = *p;
        if (xlast != S.X()) {
          if ((*S.bottom().S(0).getONSITE()).id() - 1 == Bsite) {
            wind[dim] += (double)(S.bottom().S(1).X() - S.bottom().S(0).X());
          } else if ((*S.bottom().S(2).getONSITE()).id() - 1 == Bsite) {
            wind[dim] += (double)(S.bottom().S(3).X() - S.bottom().S(2).X());
          }
          xlast = S.X();
        }
      }
    }  // end of for(xi)

    double wind2 = 0.0;
    for (int di = 0; di < LAT.D; di++) {
      double W = 0;
      for (int db = 0; db < Dim; db++) {
        W += wind[db] * LAT.vec[di][db] * LAT.L[di];
      }
      wind2 += W * W;
    }
    ACC[Wxy2].accumulate(sgn * wind2);
  }  // end of if(Dim)
}

void Measurement::setsummary() {
  using namespace Specific;
  double WDIAG = ALG.getBlock("WDIAG", (double)1.0);  //ALG.X["General"]["WDIAG" ].getDouble(); // 0.25

  std::vector<double> X(NACC);

  for (int i = 0; i < NACC; i++) {
    ACC[i].average();
    X[i] = ACC[i].mean();
  }

  double B    = LAT.BETA;
  double T    = 1.0 / B;
  double V    = LAT.NSITE;
  double invV = 1.0 / V;
  double D    = LAT.D;

  double invsign = 1.0 / X[SGN];

  for(int i=0; i<NACC; ++i){
    if(i==SGN){
      continue;
    }
    X[i] *= invsign;
  }

  Q[SIGN] = X[SGN];

  Q[ANV] = X[NV1] * invV;
  Q[ENE] = (EBASE + X[EB1] * T) * invV;

  Q[SPE] = (X[EB2] - X[EB1] * X[EB1] - X[NV1]) * invV;

  Q[LEN] = X[LE1];
  Q[XMX] = WDIAG * X[LE1] / B;

  Q[AMZU] = X[MZUA1];
  Q[BMZU] = X[MZUB1];
  Q[SMZU] = (X[MZUA2] - X[MZUA1] * X[MZUA1]) * V;
  Q[XMZU] = (X[MZUB2] - X[MZUB1] * X[MZUB1]) * B * V;

  Q[AMZS] = X[MZSA1];
  Q[BMZS] = X[MZSB1];
  Q[SMZS] = (X[MZSA2] - X[MZSA1] * X[MZSA1]) * V;
  Q[XMZS] = (X[MZSB2] - X[MZSB1] * X[MZSB1]) * B * V;

  Q[DS1]  = B * (X[NH1] - X[MZUA1] * X[EB1]) / V;
  Q[W2]   = X[Wxy2];
  Q[RHOS] = X[Wxy2] * 0.5 / D / V / B;
  Q[COMP] = Q[XMZU] / (X[MZUB1] * X[MZUB1]);

  for (int i = 0; i < NPHY; i++) {
    PHY[i].accumulate(Q[i]);
  }
}

#endif
