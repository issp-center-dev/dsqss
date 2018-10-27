#ifndef MY_RDM_H
#define MY_RDM_H

#include <random.h>
#include <fstream>
#include <iostream>
using namespace std;

class My_rdm {
  Random RND;

public:
  void outgen(std::string const& fname);
  void ingen(std::string const& fname);

  double rdm() { return RND.Uniform(); }
  int rdmi() { return RND.Int(10); }
  double rdme() { return RND.Exp(); }

  My_rdm(long seed);
  ~My_rdm();
};

#endif
