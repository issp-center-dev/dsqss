#ifndef MY_RDM_H
#define MY_RDM_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics.h>
#include <fstream>
#include <iostream>
using namespace std;

class My_rdm {
  gsl_rng *generator_gsl_rng;
  const gsl_rng_type *type_gsl_rng;

public:
  My_rdm(long seed);
  ~My_rdm();

  double rdm();
  long double rdml();

  void outgen(char *fname);
  void ingen(char *fname);

  int r01();
  int r012();

private:
  long double gsl_rng_rdm_ld();
  double gsl_rng_rdm_d();
};

#endif
