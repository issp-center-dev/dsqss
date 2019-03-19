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
#include "wavevector.hpp"
#include "serialize.hpp"

struct ACC_smag{
  Accumulator A1;
  Accumulator A2;
  Accumulator B1;
  Accumulator B2;

  void reset(){
    A1.reset(); A2.reset(); B1.reset(); B2.reset();
  }

  void average(){
    A1.average(); A2.average(); B1.average(); B2.average();
  }
};

namespace Serialize{
template <>
void save(std::ofstream & ofs, const ACC_smag& val){
  save(ofs, val.A1);
  save(ofs, val.A2);
  save(ofs, val.B1);
  save(ofs, val.B2);
}

template <>
void load(std::ifstream & ofs, ACC_smag& val){
  load(ofs, val.A1);
  load(ofs, val.A2);
  load(ofs, val.B1);
  load(ofs, val.B2);
}
}

struct PHY_smag{
  Accumulator A;
  Accumulator B;
  Accumulator S;
  Accumulator X;
  void reset(int k)
  {
    std::string names[4] = {"amzs", "bmzs", "smzs", "xmzs",};
    Accumulator* Q[4] = {&A, &B, &S, &X};
    for(int i=0; i<4; ++i){
      std::stringstream ss;
      ss << names[i];
      ss << k;
      Q[i]->reset(ss.str());
    }
  }
  void average(){
    A.average(); B.average(); S.average(); X.average();
  }

  void show(FILE* F, const char* prefix){
    A.show(F,prefix); B.show(F,prefix); S.show(F,prefix); X.show(F,prefix);
  }

#ifdef MULTI
  void allreduce(MPI_Comm comm){
    A.allreduce(comm); B.allreduce(comm); S.allreduce(comm); X.allreduce(comm);
  }
#endif
};

namespace Serialize{
template <>
void save(std::ofstream & ofs, const PHY_smag& val){
  save(ofs, val.A);
  save(ofs, val.B);
  save(ofs, val.S);
  save(ofs, val.X);
}

template <>
void load(std::ifstream & ofs, PHY_smag& val){
  load(ofs, val.A);
  load(ofs, val.B);
  load(ofs, val.S);
  load(ofs, val.X);
}
}


class Measurement {
private:
  int NACC;
  int NPHY;
  Parameter& P;
  Lattice& LAT;
  Algorithm& ALG;
  WaveVector& WV;
  std::vector<double> Q;

public:
  std::vector<Accumulator> ACC;        // accumurator of snapshot values
  std::vector<Accumulator> PHY;        // accumurator of set averages

  std::vector<ACC_smag> ACC_SMAG;   // accumurator of snapshot of stagmag
  std::vector<PHY_smag> PHY_SMAG;   // accumurator of stagmag

  double ediag;
  int nkink;

  double EBASE;

  Measurement(Parameter& P0, Lattice& L, Algorithm& A, WaveVector& W);
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

inline Measurement::Measurement(Parameter& P0, Lattice& L, Algorithm& A, WaveVector& W)
    : NACC(Specific::NACC),
      NPHY(Specific::NPHY),
      P(P0),
      LAT(L),
      ALG(A),
      WV(W),
      Q(NPHY),
      ACC(NACC),
      PHY(NPHY),
      ACC_SMAG(WV.NK),
      PHY_SMAG(WV.NK)
{
  AutoDebugDump("Measurement::Measurement");

  for (int i = 0; i < NACC; i++) {
    ACC[i].reset();
    ACC[i].set_key(Specific::ANAME[i]);
  }

  for (int i = 0; i < NPHY; i++) {
    PHY[i].reset();
    PHY[i].set_key(Specific::PNAME[i]);
  }

  for (int k=0; k < WV.NK; ++k){
    ACC_SMAG[k].reset();
    PHY_SMAG[k].reset(k);
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
  for (int i = 0; i < NACC; i++){
    ACC[i].reset();
  }
  for (int k=0; k < WV.NK; ++k){
    ACC_SMAG[k].reset();
  }
}

inline void Measurement::summary() {
  for (int i = 0; i < NPHY; i++) {
    PHY[i].average();
  }
  for (int k=0; k < WV.NK; ++k){
    PHY_SMAG[k].average();
  }
}

#ifdef MULTI
inline void Measurement::allreduce(MPI_Comm comm){
  for (int i=0; i<NPHY; i++){
    PHY[i].allreduce(comm);
  }
  for (int k=0; k < WV.NK; ++k){
    PHY_SMAG[k].allreduce(comm);
  }
}
#endif

inline void Measurement::show(FILE* F) {
  for (int i = 0; i < NPHY; i++) {
    PHY[i].show(F, "R");
  }
  for (int k=0; k < WV.NK; ++k){
    PHY_SMAG[k].show(F, "R");
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
  Serialize::save(F, ACC_SMAG);
  Serialize::save(F, PHY);
  Serialize::save(F, PHY_SMAG);
}

void Measurement::load(std::ifstream& F) {
  Serialize::load(F, ACC);
  const int NACC_ = ACC.size();
  if (NACC_ != Specific::NACC) {
    std::cerr << "ERROR: NACC is mismatched" << std::endl;
    std::cerr << "       model maybe changed." << std::endl;
    exit(1);
  }
  Serialize::load(F, ACC_SMAG);

  Serialize::load(F, PHY);
  const int NPHY_ = PHY.size();
  if (NPHY_ != Specific::NPHY) {
    std::cerr << "ERROR: NPHY is mismatched" << std::endl;
    std::cerr << "       model maybe changed." << std::endl;
    exit(1);
  }
  Serialize::load(F, PHY_SMAG);
}

void Measurement::measure(double sgn) {
  using namespace Specific;
  int NV = LAT.countVertices();

  double MZUA = 0.0;  // uniform,   tau=0
  double MZUB = 0.0;  // uniform,   integrated
  std::vector<double> MZSA(WV.NK);  // staggered, tau=0
  std::vector<double> MZSB(WV.NK);  // staggered, integrated

  const double T    = 1.0 / LAT.BETA;
  const double invV = 1.0 / LAT.NSITE;

  for (int s = 0; s < LAT.NSITE; s++) {
    Site& SITE  = LAT.S(s);
    SiteProperty& SP = SITE.Property();
    Segment& S0 = SITE.first();
    double mz0  = SP.XVALS[S0.X()];
    Site::iterator p(SITE);
    double mza0 = 0.0;

    std::vector<double> const &ph = WV.COSrk[s];

    while (!(++p).atOrigin()) {
      Segment& S = *p;
      double mz  = SP.XVALS[S.X()];
      mza0 += mz * S.length();
    }

    MZUA += mz0;
    MZUB += mza0;
    for(int k=0; k<WV.NK; ++k){
      MZSA[k] += ph[k] * mz0;
      MZSB[k] += ph[k] * mza0;
    }
  }
  MZUA *= invV;
  MZUB *= invV;
  MZUB *= T;
  for(int k=0; k<WV.NK; ++k){
    MZSA[k] *= invV;
    MZSB[k] *= invV;
    MZSB[k] *= T;
  }

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

  for(int k=0; k<WV.NK; ++k){
    ACC_SMAG[k].A1.accumulate(sgn * MZSA[k]);
    ACC_SMAG[k].A2.accumulate(sgn * MZSA[k] * MZSA[k]);
    ACC_SMAG[k].B1.accumulate(sgn * MZSB[k]);
    ACC_SMAG[k].B2.accumulate(sgn * MZSB[k] * MZSB[k]);
  }

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

  for(int k=0; k<WV.NK; ++k){
    ACC_SMAG[k].average();
  }


  Q[SIGN] = X[SGN];

  Q[ANV] = X[NV1] * invV;
  Q[ENE] = (EBASE + X[EB1] * T) * invV;

  Q[SPE] = (X[EB2] - X[EB1] * X[EB1] - X[NV1]) * invV;
  Q[SOM] = Q[SPE] * B;

  Q[LEN] = X[LE1];
  Q[XMX] = WDIAG * X[LE1] / B;

  Q[AMZU] = X[MZUA1];
  Q[BMZU] = X[MZUB1];
  Q[SMZU] = (X[MZUA2] - X[MZUA1] * X[MZUA1]) * V;
  Q[XMZU] = (X[MZUB2] - X[MZUB1] * X[MZUB1]) * B * V;

  Q[DS1]  = B * (X[NH1] - X[MZUA1] * X[EB1]) / V;
  Q[W2]   = X[Wxy2];
  Q[RHOS] = X[Wxy2] / (2*D * V * B);
  Q[RHOF] = Q[RHOS] / Q[AMZU];
  Q[COMP] = Q[XMZU] / (X[MZUB1] * X[MZUB1]);

  for (int i = 0; i < NPHY; i++) {
    PHY[i].accumulate(Q[i]);
  }

  for (int k=0; k<WV.NK; ++k){
    const double a1 = ACC_SMAG[k].A1.mean() * invsign;
    const double a2 = ACC_SMAG[k].A2.mean() * invsign;
    const double b1 = ACC_SMAG[k].B1.mean() * invsign;
    const double b2 = ACC_SMAG[k].B2.mean() * invsign;
    PHY_SMAG[k].A.accumulate(a1);
    PHY_SMAG[k].B.accumulate(a2);
    PHY_SMAG[k].S.accumulate((a2 - a1*a1)*V);
    PHY_SMAG[k].X.accumulate((b2 - b1*b1)*B*V);
  }
}

#endif
