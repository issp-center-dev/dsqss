//
// Array
//
// 今後の改良点：
//   A[i][j][k]...[z]
// のような参照の仕方を許すこと．
// そのためには，A[i] を自分自身より次元が１小さい
// Array への参照を返す関数として定義する．
// コンストラクタは再帰的に呼び出す．

#ifndef ARRAY_H
#define ARRAY_H

//######################################################################

#include <stdarg.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

namespace ARRAY {
int EOL = -1;
}

//######################################################################

class IndexSystem {
private:
  bool INI;
  std::string LBL;  // label
  int D;
  int* L;
  int N;

public:
  void init(const int d, const int* l, const std::string& LBL0 = "");

  IndexSystem() { INI = false; };

  IndexSystem(const int d, const int* l, const std::string& LBL0 = "") {
    INI = false;
    init(d, l, LBL0);
  };

  IndexSystem operator=(IndexSystem& I) {
    printf("IndexSystem::=()> Error.\n");
    printf("  ... The copy operation is prohibited.\n");
    exit(0);
  };

  IndexSystem(IndexSystem& I) {
    printf("IndexSystem::IndexSystem> Error.\n");
    printf("  ... The copy operation is prohibited.\n");
    exit(0);
  };

  ~IndexSystem() {
    if (initialized()) delete[] L;
  };

  bool initialized() const { return INI; };
  int coord(const int ist, const int d);
  void coord(const int ist, int* x);
  int dimension() {
    if (!initialized()) {
      printf("IndexSystem::dimension> Error. Not yet initialized.\n");
      exit(0);
    }
    return D;
  };
  int size() {
    if (!initialized()) {
      printf("IndexSystem::size()> Error. Not yet initialized.\n");
      exit(0);
    }
    return N;
  };
  int size(int i) {
    if (!initialized()) {
      printf("IndexSystem::size(int)> Error. Not yet initialized.\n");
      exit(0);
    }
    return L[i];
  };
  int operator()(const int* x);
  int operator()(std::vector<int> const& x);
  int operator()(const int M, va_list& ap);
  int operator()(const int M, ...);

  void dump() const {
    if (!initialized()) {
      printf(" Not yet initialized.\n");
      return;
    }
    printf(" Label = %s\n", LBL.c_str());
    printf(" Dimension = %d\n", D);
    printf(" Size = ( ");
    for (int i = 0; i < D; i++) {
      printf(" %d", L[i]);
    }
    printf(") = %d\n", N);
  };
};

//======================================================================

void IndexSystem::init(const int d, const int* l, const std::string& LBL0) {
  if (initialized()) delete[] L;
  if (d == 0) {
    printf("IndexSystem::init> Error. d = 0.\n");
    exit(0);
  }
  INI = true;
  LBL = LBL0;
  D   = d;
  L   = new int[d];
  N   = 1;
  for (int i = 0; i < d; i++) {
    N *= l[i];
    L[i] = l[i];
  }
  if (N == 0) {
    printf("IndexSystem::init> Error. N = 0.\n");
    for (int i = 0; i < d; i++)
      printf("  L[%d] = %d\n", i, L[i]);
    exit(0);
  };
}

//======================================================================

int IndexSystem::coord(const int ist, const int d) {
  if (!initialized()) {
    printf("IndexSystem::coord> Error. Not yet initialized.\n");
    exit(0);
  }
#ifdef DEB
  if (ist < 0 || ist >= N) {
    printf("IndexSystem::coord> Error. (%s)\n", LBL.c_str());
    printf("   The first argument %d is out of bounds.\n", ist);
    exit(0);
  }
  if (d < 0 || d >= D) {
    printf("IndexSystem::coord> Error. (%s)\n", LBL.c_str());
    printf("   The second argument %d is out of bounds.\n", d);
    exit(0);
  }
#endif
  int K = 1;
  for (int j = d + 1; j < D; j++) {
    K *= L[j];
  }
  int ans = ist / K;
  ans %= L[d];
  return ans;
}

//======================================================================

void IndexSystem::coord(const int ist, int* x) {
  if (!initialized()) {
    printf("IndexSystem::coord> Error. Not yet initialized.\n");
    exit(0);
  }
#ifdef DEB
  if (ist < 0 || ist >= N) {
    printf("IndexSystem::coord> Error. (%s)\n", LBL.c_str());
    printf("   The first argument %d is out of bounds.\n", ist);
    exit(0);
  }
#endif
  int I = ist;
  for (int j = D - 1; j >= 0; j--) {
    x[j] = I % L[j];
    I /= L[j];
  }
}

//======================================================================

int IndexSystem::operator()(const int* x) {
  if (!initialized()) {
    printf(
        "IndexSystem::operator()(const int*)> Error. Not yet initialized.\n");
    exit(0);
  }
  int ans = 0;

  for (int i = 0; i < D; i++) {
#ifdef DEB
    if (x[i] < 0 || x[i] >= L[i]) {
      printf("IndexSystem::id> ERROR. (%s)\n", LBL.c_str());
      printf("  The %d th argument (= %d) exceeds the bound (= %d)\n", i + 1,
             x[i], L[i] - 1);
      exit(0);
    };
#endif
    ans *= L[i];
    ans += x[i];
  }
  return ans;
}

int IndexSystem::operator()(std::vector<int> const& x) {
  if (!initialized()) {
    printf(
        "IndexSystem::operator()(const int*)> Error. Not yet initialized.\n");
    exit(0);
  }
  int ans = 0;

  for (int i = 0; i < D; i++) {
#ifdef DEB
    if (x[i] < 0 || x[i] >= L[i]) {
      printf("IndexSystem::id> ERROR. (%s)\n", LBL.c_str());
      printf("  The %d th argument (= %d) exceeds the bound (= %d)\n", i + 1,
             x[i], L[i] - 1);
      exit(0);
    };
#endif
    ans *= L[i];
    ans += x[i];
  }
  return ans;
}

//======================================================================

int IndexSystem::operator()(const int M, va_list& ap) {
  if (!initialized()) {
    printf("IndexSystem::operator()> Error. Not yet initialized.\n");
    exit(0);
  }
  int x[D];
  //int* x = new int[D]; //edit sakakura
  x[0] = M;
  for (int i = 1; i < D; i++)
    x[i] = va_arg(ap, int);
  return (*this)(x);
}

//======================================================================

int IndexSystem::operator()(const int M, ...) {
  if (!initialized()) {
    printf("IndexSystem::operator()> Error. Not yet initialized.\n");
    exit(0);
  }
  va_list ap;
  va_start(ap, M);
  int i = (*this)(M, ap);
  va_end(ap);
  return i;
}

//######################################################################

template <class C>
class Array {
private:
  std::string LBL;  // label
  int D;
  C* val;
  IndexSystem ID;
  void init(va_list& ap);

public:
  bool isDefined();
  void reset();
  void init() { reset(); };
  void init(const std::string& s, int d, ...);
  void init(int d, ...);
  Array() {
    LBL = "";
    val = 0;
  };
  Array(const std::string& LBL0) {
    LBL = LBL0;
    val = 0;
  };
  //Array(int d, ...);
  ~Array();
  void set_all(C v);
  // == edit sakakura ==
  void set_value(double* v);
  // == edit sakakura ==
  void setLabel(const char* label) { LBL = label; };
  int index(int i, int d);
  C& operator()(const int M, ...);
  C& operator()(int* x);
  C& operator()(std::vector<int> const& x);
  C& operator[](const int i);
  int size();
  int dimension();
  IndexSystem& index_system();
  void dump(char* fmt);
};

//======================================================================

template <class C>
bool Array<C>::isDefined() {
  if (val != 0) return true;
  return false;
}

//======================================================================

template <class C>
IndexSystem& Array<C>::index_system() {
  return ID;
}

//======================================================================

template <class C>
void Array<C>::init(va_list& ap) {
  reset();
  int* L = new int[D];
  int N  = 1;
  int l;
  int i = 0;
  for (i = 0; i < D; i++) {
    l = va_arg(ap, int);

    if (l == ARRAY::EOL) break;
    L[i] = l;
    N *= l;
  }
  if (N == 0) {
    printf("Array::init> Error.\n");
    printf("   ... tried to create an array with size 0.\n");
    exit(0);
  }
  if (l == ARRAY::EOL) {
    for (int j = i; j < D; j++) {
      L[j] = L[i - 1];
      N *= L[j];
    }
  }
  val = new C[N];
  ID.init(D, L, LBL);
  delete[] L;
}

//======================================================================

template <class C>
void Array<C>::init(int d, ...) {
  //  printf("Array<C>::init (1)> d= %d\n",d);
  D = d;
  va_list ap;
  va_start(ap, d);
  init(ap);
  va_end(ap);
}

//======================================================================

template <class C>
void Array<C>::init(const std::string& s, int d, ...) {
  LBL = s;
  D   = d;
  va_list ap;
  va_start(ap, d);
  init(ap);
  va_end(ap);
}

//======================================================================
/*
  template<class C>
  Array<C>::Array(int d, ...) {
  LBL = "";
  D = d;
  va_list ap;
  va_start( ap , d );
  init( ap );
  va_end( ap );
  };
*/
//======================================================================

template <class C>
void Array<C>::reset() {
  if (val != 0) {
    delete[] val;
    val = 0;
  }
}

//======================================================================

template <class C>
Array<C>::~Array() {
  //  printf("*** Destroying Array (%s)\n", LBL.c_str());
  reset();
}

//======================================================================

template <class C>
void Array<C>::set_all(C v) {
  for (int i = 0; i < size(); i++) {
    val[i] = v;
  }
}

// == edit sakakura == //
//======================================================================

template <class C>
void Array<C>::set_value(double* v) {
  for (int i = 0; i < size(); i++) {
    val[i] = v[i];
  }
}

//======================================================================
// == edit sakakura == //

//======================================================================

template <class C>
int Array<C>::index(int i, int d) {
  return ID.coord(i, d);
}

//======================================================================

template <class C>
C& Array<C>::operator()(const int M, ...) {
  va_list ap;
  va_start(ap, M);
  int i = ID(M, ap);
  va_end(ap);
  return val[i];
}

//======================================================================

template <class C>
C& Array<C>::operator[](const int i) {
#ifdef DEB
  if (i < 0 || i >= size()) {
    printf("Array::operator[]> Error in array \"%s\".\n", LBL.c_str());
    printf("  The index (=%d) is out of the bounds [0,%d).\n", i, size());
    exit(0);
  }
#endif
  return val[i];
}

//======================================================================

template <class C>
C& Array<C>::operator()(int* x) {
  int id = ID(x);
#ifdef DEB
  if (id < 0 || id > size()) {
    printf("Array::operator()> Error.\n");
    printf("  Out of bounds.\n");
    exit(0);
  }
#endif
  return val[id];
}

template <class C>
C& Array<C>::operator()(std::vector<int> const& x) {
  int id = ID(x);
#ifdef DEB
  if (id < 0 || id > size()) {
    printf("Array::operator()> Error.\n");
    printf("  Out of bounds.\n");
    exit(0);
  }
#endif
  return val[id];
}


//======================================================================

template <class C>
int Array<C>::size() {
  return ID.size();
}

//======================================================================

template <class C>
int Array<C>::dimension() {
  return ID.dimension();
}

//======================================================================

template <class C>
void Array<C>::dump(char* fmt) {
  for (int i = 0; i < size(); i++) {
    printf("%s", LBL.c_str());
    for (int d = 0; d < dimension(); d++) {
      printf("[%d]", ID.coord(i, d));
    }
    printf(" = ");
    printf(fmt, (*this)[i]);
    printf("\n");
  }
}

#endif
