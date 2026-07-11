#ifndef DSQSS_GENERATORS_SITE_OPERATORS_H
#define DSQSS_GENERATORS_SITE_OPERATORS_H
#include <vector>

#include "matrix.h"

//============================================================================
//    One-site ladder-operator algebra shared by the spin (hamgen_H)
//    and boson (hamgen_B) generators. The two models differ only in
//    the raising-operator matrix elements and the diagonal of Z.
//============================================================================

class SiteOperator {
 public:
  int K;  // 2S for spins / the maximum occupation for bosons
  int D;  // = K + 1, the local dimension
  cmatrix I;
  cmatrix UP;
  cmatrix DN;
  cmatrix X;
  cmatrix Y;
  cmatrix Z;

 protected:
  explicit SiteOperator(int K0) : K(K0), D(K0 + 1) {}

  // up[i] = <i+1| raising operator |i>,  z[i] = <i| Z |i>
  void init(const std::vector<double>& up, const std::vector<double>& z) {
    I.resize(D, D);
    UP.resize(D, D);
    DN.resize(D, D);
    X.resize(D, D);
    Y.resize(D, D);
    Z.resize(D, D);
    I.zero();
    UP.zero();
    DN.zero();
    X.zero();
    Y.zero();
    Z.zero();
    for (int i = 0; i < D - 1; i++) {
      UP.re(i + 1, i) = up[i];
    }
    DN = t(UP);
    cmatrix temp = UP + DN;
    X = 0.5 * temp;
    Y = ((-0.5) * IUNIT) * (UP - DN);
    for (int i = 0; i < D; i++) {
      I.re(i, i) = 1.0;
      Z.re(i, i) = z[i];
    }
  }
};

class HeisenbergSpin : public SiteOperator {
 public:
  explicit HeisenbergSpin(int K0) : SiteOperator(K0) {
    std::vector<double> up(D - 1), z(D);
    for (int i = 0; i < D - 1; i++) {
      up[i] = sqrt((double)((i + 1) * (K - i)));
    }
    for (int i = 0; i < D; i++) {
      z[i] = -0.5 * (double)K + (double)i;
    }
    init(up, z);
  }
};

class BosonOperator : public SiteOperator {
 public:
  explicit BosonOperator(int K0) : SiteOperator(K0) {
    std::vector<double> up(D - 1), z(D);
    for (int i = 0; i < D - 1; i++) {
      up[i] = sqrt((double)(i + 1));
    }
    for (int i = 0; i < D; i++) {
      z[i] = (double)i;
    }
    init(up, z);
  }
};

//============================================================================
//    Kronecker lift of the one-site operators onto an NSITE-site
//    system (replaces HeisenbergSpinSet / BosonOperatorSet).
//============================================================================

class SiteOperatorSet {
 public:
  int DS;     // the one-site local dimension
  int K;      // as in SiteOperator
  int NSITE;  // the number of sites
  int DIM;    // the dimension of the whole Hilbert space

  std::vector<cmatrix> UP;
  std::vector<cmatrix> DN;
  std::vector<cmatrix> X;
  std::vector<cmatrix> Y;
  std::vector<cmatrix> Z;
  cmatrix I;

  SiteOperatorSet(const SiteOperator& S, int NSITE0) {
    K = S.K;
    NSITE = NSITE0;
    DS = S.D;

    UP.resize(NSITE);
    DN.resize(NSITE);
    X.resize(NSITE);
    Y.resize(NSITE);
    Z.resize(NSITE);

    DIM = 1;
    for (int i = 0; i < NSITE; i++) {
      DIM *= DS;
      cmatrix up, dn, x, y, z;
      up.unity();
      dn.unity();
      x.unity();
      y.unity();
      z.unity();
      for (int j = 0; j < NSITE; j++) {
        const cmatrix& lift_up = (i == j) ? S.UP : S.I;
        const cmatrix& lift_dn = (i == j) ? S.DN : S.I;
        const cmatrix& lift_x = (i == j) ? S.X : S.I;
        const cmatrix& lift_y = (i == j) ? S.Y : S.I;
        const cmatrix& lift_z = (i == j) ? S.Z : S.I;
        up = lift_up ^ up;
        dn = lift_dn ^ dn;
        x = lift_x ^ x;
        y = lift_y ^ y;
        z = lift_z ^ z;
      }
      UP[i] = up;
      DN[i] = dn;
      X[i] = x;
      Y[i] = y;
      Z[i] = z;
    }
    I.resize(DIM);
    I.identity();
  }
};

#endif  // DSQSS_GENERATORS_SITE_OPERATORS_H
