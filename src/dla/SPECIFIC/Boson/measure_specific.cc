#include "../../util.hpp"

void Measurement::measure() {
  using namespace Specific;
  int NV = LAT.countVertices();

  ACC[NV1].accumulate(((double)NV));


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
    int mt      = SITE.getMTYPE();
    double ph   = phase[mt];
    Segment& S0 = SITE.first();
    double mz0  = dvals[S0.X()];
    Site::iterator p(SITE);
    double mza0 = 0.0;

    while (!(++p).atOrigin()) {
      Segment& S = *p;
      double mz  = dvals[S.X()];
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
#ifndef NORM
  MZUB *= T;
  MZSB *= T;
#endif

  ACC[MZUA1].accumulate(MZUA);
  ACC[MZUA2].accumulate(MZUA * MZUA);
  ACC[MZUB1].accumulate(MZUB);
  ACC[MZUB2].accumulate(MZUB * MZUB);
  ACC[MZSA1].accumulate(MZSA);
  ACC[MZSA2].accumulate(MZSA * MZSA);
  ACC[MZSB1].accumulate(MZSB);
  ACC[MZSB2].accumulate(MZSB * MZSB);

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
      p[i]++;
      tau[i] = p[i]->topTime();
      x[i]   = p[i]->X();
    }

    double t = 0.0;

#ifdef NORM
    while (t < 1.0) {
#else
    while (t < LAT.BETA) {
#endif
      int it = util::min_index(tau);
#ifdef NORM
      EBSAMP -= (tau[it] - t) * IP.VertexDensity(x) * LAT.BETA;
#else
      EBSAMP -= (tau[it] - t) * IP.VertexDensity(x);
#endif

      if (p[it]->top().isTerminal()) break;
      t = tau[it];
      p[it]++;
      tau[it] = p[it]->topTime();
      x[it]   = p[it]->X();
    }
  }

  ACC[EB1].accumulate(EBSAMP);
  ACC[EB2].accumulate(EBSAMP * EBSAMP);
  ACC[NH1].accumulate(MZUA * EBSAMP);

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
    ACC[Wxy2].accumulate(wind2);
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

  Q[ANV] = X[NV1] * invV;
  Q[ENE] = (EBASE + X[EB1] * T) * invV;

  Q[SPE] = (X[EB2] - X[EB1] * X[EB1] - X[NV1]) * invV;

#ifdef NORM
  Q[LEN] = X[LE1] * B;
  Q[XMX] = WDIAG * X[LE1];
#else
  Q[LEN] = X[LE1];
  Q[XMX] = WDIAG * X[LE1] / B;
#endif

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
