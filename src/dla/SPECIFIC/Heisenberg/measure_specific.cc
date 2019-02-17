#include "../../util.hpp"

void Measurement::measure(double sgn) {
  using namespace Specific;
  int NV = LAT.countVertices();

  ACC[NV1].accumulate(((double)NV));

  double MZUA = 0.0;  // uniform,   tau=0
  double MZUB = 0.0;  // uniform,   integrated
  double MZSA = 0.0;  // staggered, tau=0
  double MZSB = 0.0;  // staggered, integrated

  const double T = 1.0/LAT.BETA;
  const double invV = 1.0/LAT.NSITE;

  std::vector<double> phase(LAT.NSMTYPE);
  for(int m = 0; m < LAT.NSMTYPE; ++m){
    phase[m] = std::cos(2.0*M_PI*m/LAT.NSMTYPE);
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
  MZUB *= T;
  MZSB *= T;

  double EBSAMP = -(double)NV;

  for (int b = 0; b < LAT.NINT; b++) {
    Interaction& I          = LAT.I(b);
    InteractionProperty& IP = I.property();
    //VertexProperty& VP = IP.getVertexProperty();
    int NBODY         = IP.NBODY;
    std::vector<double> tau(NBODY);
    std::vector<int> x(NBODY);
    std::vector<int> x2(2*NBODY);
    std::vector<Site::iterator> p(NBODY);

    for (int i = 0; i < NBODY; i++) {
      Site& S = I.site(i);
      p[i].init(S);
      ++p[i];
      tau[i] = p[i]->topTime();
      x[i]   = p[i]->X();
    }

    double t = 0.0;
    int it;

    while (t < LAT.BETA) {
      it = util::min_index(tau);
      EBSAMP -= (tau[it] - t) * IP.VertexDensity(x);

      if (p[it]->top().isTerminal()) break;
      t = tau[it];
      ++p[it];
      tau[it] = p[it]->topTime();
      x[it]   = p[it]->X();
    }
  }

  ACC[SGN].accumulate(sgn);

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
}

void Measurement::setsummary() {
  using namespace Specific;
  const double WDIAG = ALG.getBlock("WDIAG", (double)1.0);  //ALG.X["General"]["WDIAG" ].getDouble(); // 0.25

  std::vector<double> X(NACC);

  for (int i = 0; i < NACC; i++){
    ACC[i].average();
    X[i] = ACC[i].mean();
  }

  const double B = LAT.BETA;
  const double T = 1.0/B;
  const double V = LAT.NSITE;
  const double invV = 1.0/V;
  const double D = LAT.D;

  double invsign = 1.0/X[SGN];
  Q[SIGN] = X[SGN];
  Q[ANV] = invsign * X[NV1] * invV;
  Q[ENE] = invsign * (EBASE + X[EB1] / B) * invV;

  Q[SPE] = invsign * (X[EB2] - X[EB1] * X[EB1] - X[NV1]) * invV;

  Q[LEN] = invsign * X[LE1];
  Q[XMX] = invsign * WDIAG * X[LE1] * T;

  Q[AMZU] = invsign * X[MZUA1];
  Q[BMZU] = invsign * X[MZUB1];
  Q[SMZU] = invsign * (X[MZUA2] - X[MZUA1] * X[MZUA1]) * V;
  Q[XMZU] = invsign * (X[MZUB2] - X[MZUB1] * X[MZUB1]) * B * V;

  Q[AMZS] = invsign * X[MZSA1];
  Q[BMZS] = invsign * X[MZSB1];
  Q[SMZS] = invsign * (X[MZSA2] - X[MZSA1] * X[MZSA1]) * V;
  Q[XMZS] = invsign * (X[MZSB2] - X[MZSB1] * X[MZSB1]) * B * V;

  for (int i = 0; i < NPHY; i++)
    PHY[i].accumulate(Q[i]);
}
