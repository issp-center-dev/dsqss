
//============================================================================
//    Spin Matrices
//============================================================================

class HeisenbergSpin {
public:
  int K;
  int D;
  cmatrix I;
  cmatrix UP;
  cmatrix DN;
  cmatrix X;
  cmatrix Y;
  cmatrix Z;
  HeisenbergSpin(int);
  ~HeisenbergSpin(){};
};

//----------------------------------------------------------------------------

HeisenbergSpin::HeisenbergSpin(int K0) {
  K = K0;  // K = 2 * S
  D = K0 + 1;
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
    UP.re(i + 1, i) = sqrt((double)((i + 1) * (K - i)));
  }
  DN = t(UP);
  //  X = 0.5 * (UP + DN);
  cmatrix temp = UP + DN;
  X            = 0.5 * temp;
  Y            = ((-0.5) * IUNIT) * (UP - DN);
  for (int i = 0; i < D; i++) {
    I.re(i, i) = 1.0;
    Z.re(i, i) = -0.5 * (double)K + (double)i;
  }
}

//----------------------------------------------------------------------------

class HeisenbergSpinSet {
public:
  int DS;     // the dimension of the 1-spin Hilbert space
  int N;      // "N" in SU(N)
  int K;      // the number of bosons on each site
  int NSITE;  // the number of spins
  int DIM;    // the dimension of the whole Hilbert space

  cmatrix* X;
  cmatrix* Y;
  cmatrix* Z;
  cmatrix I;

  HeisenbergSpinSet(int K0, int NSITE0) {
    printf("SpinSet> start.\n");

    K     = K0;
    NSITE = NSITE0;

    HeisenbergSpin S(K);

    DS = S.D;
    X  = new cmatrix[NSITE];
    Y  = new cmatrix[NSITE];
    Z  = new cmatrix[NSITE];

    printf("  definig spins ...\n");
    DIM = 1;

    for (int i = 0; i < NSITE; i++) {
      DIM *= (DS);
      cmatrix x;
      cmatrix y;
      cmatrix z;
      x.unity();
      y.unity();
      z.unity();
      for (int j = 0; j < NSITE; j++) {
        if (i == j) {
          x = (S.X) ^ x;
          y = (S.Y) ^ y;
          z = (S.Z) ^ z;
        } else {
          x = (S.I) ^ x;
          y = (S.I) ^ y;
          z = (S.I) ^ z;
        }

        X[i] = x;
        Y[i] = y;
        Z[i] = z;
      }
      printf("  ... done.\n");

      I.resize(DIM);
      I.identity();

      printf("SpinSet> end.\n");
    };
  };
};
