#ifndef DISPLACEMENTS_H
#define DISPLACEMENTS_H

#include <cstdio>
#include <fstream>
#include <sstream>
#include <cmath>
#include "debug.hpp"
#include "accumulator.hpp"
#include "parameter.hpp"
#include "xml.h"

struct Displacements {
  bool to_be_calc;

  int nr;
  std::vector<int> nrs;
  std::vector<std::vector<int> > r_id;

  Displacements(std::string const &filename, int nsites);
};

Displacements::Displacements(std::string const &filename, int nsites)
{
  AutoDebugDump("Displacements::Displacements");
  if (filename.length() > 0) {
    XML::Block X(filename.c_str());
    int Nline = X["NumberOfElements"].getInteger();

    nr = X["NumberOfKinds"].getInteger();

    nrs.resize(nr,0);

    for (int i = 0; i < nsites; i++) {
      r_id.push_back(std::vector<int>(nsites, nr));
    }

    int count = 0;
    for (int i = 0; i < X.NumberOfBlocks(); i++) {
      XML::Block& B = X[i];
      if (B.getName() == "CF") {
        int icf_  = B.getInteger(0);
        int isite = B.getInteger(1);
        int jsite = B.getInteger(2);
        //cout<<isite<<"-"<<jsite<<endl;
        r_id[isite][jsite] = icf_;
        nrs[icf_]++;
        count++;
      }
    }

    if (count != Nline) {
      std::cout << "ERROR Nline( " << Nline << " ) != count( " << count << " )" << std::endl;
      exit(0);
    }
  }
}

#endif  //DISPLACEMENTS_H
