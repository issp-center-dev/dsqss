#ifndef DISPLACEMENT_H
#define DISPLACEMENT_H

#include <cstdio>
#include <fstream>
#include <sstream>
#include <cmath>
#include "debug.hpp"
#include "accumulator.hpp"
#include "parameter.hpp"
#include "xml.h"

struct Displacement {
  bool defined;
  int NSITES;
  int nkinds;

  std::vector<int> NR; // the number of pairs with the same dR
  std::vector<std::vector<int> > IR; // IR[isite][jsite] == disp_index

  Displacement(Parameter const& param);
  void read(XML::Block const& X);
};

Displacement::Displacement(Parameter const& param) : defined(false)
{
  AutoDebugDump("Displacement::Displacement");
  if (param.DISPFILE.length() > 0) {
    defined = true;
    XML::Block X(param.DISPFILE.c_str());

    NSITES = X["NumberOfSites"].getInteger();
    nkinds = X["NumberOfKinds"].getInteger();

    NR.resize(nkinds,0);

    for (int i = 0; i < NSITES; i++) {
      IR.push_back(std::vector<int>(NSITES, nkinds));
    }

    for (int i = 0; i < X.NumberOfBlocks(); i++) {
      XML::Block& B = X[i];
      if (B.getName() == "R") {
        int kind  = B.getInteger(0);
        int isite = B.getInteger(1);
        int jsite = B.getInteger(2);
        IR[isite][jsite] = kind;
        NR[kind]++;
      }
    }
  }
};

#endif  //DISPLACEMENT_H
