
//============================================================================
//    Exact Calculation of Finite Size Spin Systems
//============================================================================

#include <stdint.h>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <fstream>
#include <map>
#include <string>
#include <vector>  //only for ?geev, ?gegv, etc.

//#include <ctime>

//=============================================================================
#ifdef __INTEL_COMPILER
#define DSQSS_INT MKL_INT
#define MKL_Complex16 std::complex<double>
#else  //__INTEL_COMPILER
#define DSQSS_INT int
#endif  //__INTEL_COMPILER

//=============================================================================

#ifdef __INTEL_COMPILER
#include <mkl.h>
#define dsyev_ DSYEV
#define dgemm_ DGEMM
#else   //__INTEL_COMPILER
extern "C" {

void dsyev_(const char* jobz, const char* uplo, const DSQSS_INT* N, double* a,
            const DSQSS_INT* lda, double* w, double* work,
            const DSQSS_INT* lwork, DSQSS_INT* info);

void dgemm_(const char* transa, const char* transb, const DSQSS_INT* M,
            const DSQSS_INT* N, const DSQSS_INT* k, const double* alpha,
            const double* a, const DSQSS_INT* lda, const double* b,
            const DSQSS_INT* ldb, const double* beta, double* c,
            const DSQSS_INT* ldc);
}
#endif  //__INTEL_COMPILER

//============================================================================
class dgematrix {
 private:
  vector<double> index;

 public:
  int n, m;

  ////////////////
  dgematrix(int _n, int _m) {
    resize(_m, _n);
    for (int i = 0; i < m * n; i++) {
      index[i] = 0.0;
    }
  };

  dgematrix() {
    n = 0;
    m = 0;
  };
  //  inline dgematrix(const _dgematrix&);
  ~dgematrix() { index.clear(); };
  //////////////////

  void resize(int _m, int _n) {
    n = _n;
    m = _m;
    index.resize(_m * _n);
  };

  inline void clear() { index.clear(); };

  inline dgematrix& zero() {
    for (long i = 0; i < m * n; i++) {
      index[i] = 0.0;
    }
    return *this;
  }

  inline dgematrix& identity() {
    for (int i = 0; i < m * n; i++) {
      index[i] = 0.0;
    }
    for (int i = 0; i < m; i++) {
      operator()(i, i) = 1.0;
    }
    return *this;
  }

  inline dgematrix& operator*=(const dgematrix&);
  inline double& operator()(const int& i, const int& j) {
    return index[i + j * m];
  };
  inline double operator()(const int& i, const int& j) const {
    return index[i + j * m];
  };
  inline dgematrix& operator=(const dgematrix& A) {
    resize(A.m, A.n);

    for (int i = 0; i < A.m; i++) {
      for (int j = 0; j < A.n; j++) {
        index[i + j * m] = A(i, j);
      }
    }

    return *this;
  };
  inline dgematrix& operator+=(const dgematrix& A) {
    for (int i = 0; i < A.m; i++) {
      for (int j = 0; j < A.n; j++) {
        index[i + j * m] += A(i, j);
      }
    }

    return *this;
  };
  inline dgematrix& operator-=(const dgematrix& A) {
    for (int i = 0; i < A.m; i++) {
      for (int j = 0; j < A.n; j++) {
        index[i + j * m] -= A(i, j);
      }
    }

    return *this;
  };
  ////////////
};

// transposed matrix
inline dgematrix t(const dgematrix& A) {
  dgematrix C(A.n, A.m);

  for (int i = 0; i < C.m; i++) {
    for (int j = 0; j < C.n; j++) {
      C(i, j) = A(j, i);
    }
  }

  return C;
};

//////////////

dgematrix operator+(const dgematrix& A, const dgematrix& B) {
  dgematrix C(A.m, A.n);

  for (int i = 0; i < A.m; i++) {
    for (int j = 0; j < A.n; j++) {
      C(i, j) = A(i, j) + B(i, j);
    }
  }

  return C;
};

dgematrix operator-(const dgematrix& A, const dgematrix& B) {
  dgematrix C(A.m, A.n);

  for (int i = 0; i < A.m; i++) {
    for (int j = 0; j < A.n; j++) {
      C(i, j) = A(i, j) - B(i, j);
    }
  }

  return C;
};
dgematrix operator*(const double& a, const dgematrix& A) {
  dgematrix C(A.m, A.n);

  for (int i = 0; i < A.m; i++) {
    for (int j = 0; j < A.n; j++) {
      C(i, j) = a * A(i, j);
    }
  }

  return C;
};

dgematrix operator*(const dgematrix& dA, const dgematrix& dB) {
  DSQSS_INT M = dA.m;
  DSQSS_INT N = dB.n;
  DSQSS_INT K = dA.n;
  double *A, *B, *C;

  A = new double[dA.m * dA.n];
  B = new double[dB.m * dB.n];
  C = new double[dA.m * dB.n];

  dgematrix dC(dA.m, dB.n);

  double alpha = 1.0, beta = 0.0;

  char transa = 'n';
  char transb = 'n';

  for (int i = 0; i < dA.m; i++) {
    for (int j = 0; j < dA.n; j++) {
      A[i + j * dA.m] = dA(i, j);
    }
  }

  for (int i = 0; i < dB.m; i++) {
    for (int j = 0; j < dB.n; j++) {
      B[i + j * dB.m] = dB(i, j);
    }
  }

  DSQSS_INT lda = dA.m;
  DSQSS_INT ldb = dA.n;

  //  cout<<"M, N, K ="<< M <<" "<<N<<" "<<K<<" "<<endl;

  dgemm_(&transa, &transb, &M, &N, &K, &alpha, A, &lda, B, &ldb, &beta, C, &M);

  for (int i = 0; i < dA.m; i++) {
    for (int j = 0; j < dB.n; j++) {
      dC(i, j) = C[i + j * dA.m];
    }
  }

  delete[] A;
  delete[] B;
  delete[] C;

  return dC;
};

//
class dsymatrix {
 private:
  double* index;

 public:
  int n;

  inline double& operator()(const int& i, const int& j) {
    return index[i + j * n];
  };
  inline double operator()(const int& i, const int& j) const {
    return index[i + j * n];
  };

  DSQSS_INT dsyev(vector<double>&);

  dsymatrix(int N) {
    n = N;
    index = new double[N * N];
  };

  dsymatrix() { n = 0; };

  ~dsymatrix() { delete[] index; };
};

DSQSS_INT dsymatrix::dsyev(vector<double>& E) {
  ////////////////////
  DSQSS_INT info;
  char jobz = 'V';
  char uplo = 'I';
  DSQSS_INT w_size = -1;
  DSQSS_INT size = n;
  DSQSS_INT size2 = size * size;
  double* work;
  work = new double[size * 20];
  double* vr;
  vr = new double[size2];
  double* wr;
  wr = new double[size];
  //  wr //eigen value
  //  vr //eigen vector

  for (int i = 0; i < (int)size2; i++) vr[i] = index[i];

  dsyev_(&jobz, &uplo, &size, vr, &size, wr, work, &w_size, &info);

  for (int i = 0; i < (int)size; i++) E[i] = wr[i];
  for (int i = 0; i < (int)size2; i++) index[i] = vr[i];
  ///////////////

  delete[] work;
  delete[] vr;
  delete[] wr;

  return info;
};

//============================================================================

complex<double> IUNIT(0.0, 1.0);

//============================================================================
//    Display
//============================================================================

void dump(const vector<double>& V) {
  printf("\n");
  for (int i = 0; i < V.size(); i++) {
    printf(" %8.3f", V[i]);
  }
  printf("\n");
}

//----------------------------------------------------------------------------

void dump(char* s, const vector<double>& V) {
  printf("\n");
  printf("%s\n", s);
  dump(V);
}

//----------------------------------------------------------------------------

void dump(const dgematrix& A, int Mmax = 10) {
  int M = A.m;
  if (M > Mmax) M = Mmax;
  int N = A.n;
  if (N > Mmax) N = Mmax;
  printf("\n");
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      printf(" %8.3f", A(i, j));
      //    printf(" %2d", (int)(A(i,j)+0.1));
    }
    printf("\n");
  }
}

//----------------------------------------------------------------------------

void dump01(const dgematrix& A, int Mmax = 64) {
  int M = A.m;
  if (M > Mmax) M = Mmax;
  int N = A.n;
  if (N > Mmax) N = Mmax;
  printf("\n");
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      char x = '.';
      if (abs(A(i, j)) > 1.0e-8) x = 'X';
      printf("%1c", x);
    }
    printf("\n");
  }
}

//----------------------------------------------------------------------------

void dump(char* s, const dgematrix& A) {
  printf("\n");
  printf("%s\n", s);
  dump(A);
}

//============================================================================
//    Diagonalization
//============================================================================

void diagonalize(dsymatrix& A, vector<double>& E, dgematrix& U) {
  dsymatrix A0 = A;

  A0.dsyev(E);

  for (int i = 0; i < A.n; i++) {
    for (int j = 0; j < A.n; j++) {
      U(i, j) = A0(i, j);
    }
  }

  return;
}

//----------------------------------------------------------------------------

void diagonalize(dgematrix& A, vector<double>& E, dgematrix& U) {
  dsymatrix A0(A.n);
  for (int i = 0; i < A.n; i++) {
    for (int j = 0; j <= i; j++) {
      A0(i, j) = A(i, j);
    }
  }
  diagonalize(A0, E, U);
}

//============================================================================
//    Tensor Product
//============================================================================

dgematrix operator^(const dgematrix& A, const dgematrix& B) {
  int _m = A.m * B.m;
  int _n = A.n * B.n;
  dgematrix C(_m, _n);

  for (int i0 = 0; i0 < A.m; i0++) {
    for (int i1 = 0; i1 < B.m; i1++) {
      int i = i0 + A.m * i1;

      for (int j0 = 0; j0 < A.n; j0++) {
        for (int j1 = 0; j1 < B.n; j1++) {
          int j = j0 + A.n * j1;
          C(i, j) = A(i0, j0) * B(i1, j1);
        }
      }
    }
  }
  return C;
}

//============================================================================
//    Complex Matrix
//============================================================================

class cmatrix {
 public:
  long m;
  long n;

  dgematrix re;
  dgematrix im;

  void resize(const long m0, const long n0) {
    m = m0;
    n = n0;
    re.resize(m, n);
    im.resize(m, n);
  };

  void resize(const long n0) { resize(n0, n0); }

  cmatrix(){};
  cmatrix(const long n0) { resize(n0, n0); };
  cmatrix(const long m0, const long n0) { resize(m0, n0); };
  cmatrix(const cmatrix& X) { resize(X.m, X.n); };

  cmatrix& operator+=(const cmatrix& A) {
    if (m != A.m) {
      printf("cmatrix::operator+= >> ERROR;");
      exit(0);
    }
    if (n != A.n) {
      printf("cmatrix::operator+= >> ERROR;");
      exit(0);
    }
    re += A.re;
    im += A.im;
    return *this;
  };

  cmatrix& operator-=(const cmatrix& A) {
    if (m != A.m) {
      printf("cmatrix::operator+= >> ERROR;");
      exit(0);
    }
    if (n != A.n) {
      printf("cmatrix::operator+= >> ERROR;");
      exit(0);
    }
    re -= A.re;
    im -= A.im;
    return *this;
  };

  cmatrix& operator=(const cmatrix& A) {
    m = A.m;
    n = A.n;
    re = A.re;
    im = A.im;
    return *this;
  };

  void clear() {
    re.clear();
    im.clear();
  };

  void zero();

  void unity() {
    resize(1, 1);
    re(0, 0) = 1.0;
    im(0, 0) = 0.0;
  };

  void identity() {
    re.identity();
    im.identity();
  }

  void dump(int Mmax);
  void dump(char*);
  void dump01(int Mmax);
};

//----------------------------------------------------------------------------

void cmatrix::zero() {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      re(i, j) = 0.0;
      im(i, j) = 0.0;
    }
  }
}

//----------------------------------------------------------------------------

void cmatrix::dump(int Mmax = 10) {
  int M = m;
  if (M > Mmax) M = Mmax;
  int N = n;
  if (N > Mmax) N = Mmax;
  printf("\n");
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      //    if ( j!= 0) printf(" |");
      printf(" %6.3f", re(i, j));
      //    printf(" %4.1f %4.1f", re(i,j), im(i,j));
    }
    printf("\n");
  }
}

//----------------------------------------------------------------------------

void cmatrix::dump01(int Mmax = 64) {
  int M = m;
  if (M > Mmax) M = Mmax;
  int N = n;
  if (N > Mmax) N = Mmax;
  printf("\n");
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      char x = '.';
      if (re(i, j) * re(i, j) + im(i, j) * im(i, j) > 1.0e-16) x = 'X';
      printf("%1c", x);
    }
    printf("\n");
  }
}

//----------------------------------------------------------------------------

void cmatrix::dump(char* s) {
  printf("%s\n", s);
  dump();
}

//----------------------------------------------------------------------------

cmatrix operator+(const cmatrix& A, const cmatrix& B) {
  cmatrix C(A.m, B.n);
  C.re = A.re + B.re;
  C.im = A.im + B.im;
  return C;
}

//----------------------------------------------------------------------------

cmatrix operator-(const cmatrix& A, const cmatrix& B) {
  cmatrix C(A.m, B.n);
  C.re = A.re - B.re;
  C.im = A.im - B.im;
  return C;
}

//----------------------------------------------------------------------------

cmatrix operator*(const cmatrix& A, const cmatrix& B) {
  cmatrix C(A.m, B.n);
  C.re = A.re * B.re - A.im * B.im;
  C.im = A.re * B.im + A.im * B.re;
  return C;
}

//----------------------------------------------------------------------------

cmatrix operator*(const double a, const cmatrix& A) {
  cmatrix C(A.m, A.n);
  C.re = a * A.re;
  C.im = a * A.im;
  return C;
}

//----------------------------------------------------------------------------

cmatrix operator*(const complex<double> c, const cmatrix& A) {
  cmatrix C(A.m, A.n);
  C.re = c.real() * A.re - c.imag() * A.im;
  C.im = c.real() * A.im + c.imag() * A.re;
  return C;
}

//----------------------------------------------------------------------------

cmatrix t(const cmatrix& A) {
  cmatrix C(A.n, A.m);
  C.re = t(A.re);
  C.im = t(A.im);
  return C;
}

//----------------------------------------------------------------------------

cmatrix operator^(cmatrix& A, cmatrix& B) {
  int m = A.m * B.m;
  int n = A.n * B.n;
  cmatrix C(m, n);
  C.re = ((A.re) ^ (B.re)) - ((A.im) ^ (B.im));
  C.im = ((A.re) ^ (B.im)) + ((A.im) ^ (B.re));
  return C;
}
