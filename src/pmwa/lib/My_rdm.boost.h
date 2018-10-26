#ifndef MY_RDM_H
#define MY_RDM_H

#include <boost/random.hpp>
#include <fstream>
#include <iostream>
using namespace std;
//using namespace boost;

class My_rdm {
  boost::mt19937 gen_d;
  boost::uniform_real<> dst_d;  // [0,1)
  boost::variate_generator<boost::mt19937, boost::uniform_real<> > rdm_d;

public:
  void outgen(char *fname);
  void ingen(char *fname);

  double rdm() { return rdm_d(); }

  My_rdm(long seed);
  ~My_rdm();
};

class My_rdms {
  boost::mt19937 gen_s;
  boost::uniform_smallint<> dst_s;  // [0,iMax]
  boost::variate_generator<boost::mt19937, boost::uniform_smallint<> > rdm_s;

public:
  void outgen(char *fname);
  void ingen(char *fname);

  short rdm() { return rdm_s(); }

  My_rdms(long seed, short iMax);
  ~My_rdms();
};

class My_rdmi {
  boost::mt19937 gen_i;
  boost::uniform_int<> dst_i;  // [0,iMax]
  boost::variate_generator<boost::mt19937, boost::uniform_int<> > rdm_i;

public:
  void outgen(char *fname);
  void ingen(char *fname);

  int rdm() { return rdm_i(); }

  My_rdmi(long seed, long iMax);
  ~My_rdmi();
};

#endif
