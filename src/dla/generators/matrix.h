#ifndef DSQSS_GENERATORS_MATRIX_H
#define DSQSS_GENERATORS_MATRIX_H

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

// The classes below use unqualified std names; historically this was
// inherited from `using namespace std;` in the including .cc file.
using namespace std;

//#include <ctime>

//=============================================================================

//============================================================================
class dgematrix {
 private:
  vector<double> index;

 public:
  int n, m;

  ////////////////
  dgematrix(int _m, int _n) {
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
  // Plain matrix product. The matrices handled by the generators are
  // tiny (pair Hamiltonians), so no BLAS is needed.
  if (dA.n != dB.m) {
    printf("dgematrix::operator* >> ERROR: shape mismatch\n");
    exit(1);
  }
  dgematrix dC(dA.m, dB.n);
  for (int i = 0; i < dA.m; i++) {
    for (int j = 0; j < dB.n; j++) {
      double s = 0.0;
      for (int k = 0; k < dA.n; k++) {
        s += dA(i, k) * dB(k, j);
      }
      dC(i, j) = s;
    }
  }
  return dC;
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
    im.zero();
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

cmatrix operator^(const cmatrix& A, const cmatrix& B) {
  int m = A.m * B.m;
  int n = A.n * B.n;
  cmatrix C(m, n);
  C.re = ((A.re) ^ (B.re)) - ((A.im) ^ (B.im));
  C.im = ((A.re) ^ (B.im)) + ((A.im) ^ (B.re));
  return C;
}

#endif  // DSQSS_GENERATORS_MATRIX_H
