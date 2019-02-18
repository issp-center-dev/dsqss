#ifndef WAVEVECTOR_H
#define WAVEVECTOR_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include "debug.hpp"
#include "xml.h"


struct Wavevector {
  int nk;
  std::vector<std::vector<double> > COSrk;
  std::vector<std::vector<double> > SINrk;

  Wavevector(std::string const & filename);
};

Wavevector::Wavevector(std::string const & filename) : nk(0)
{
  AutoDebugDump("WAVEVECTOR::WAVEVECTOR");
  if (filename.length() > 0) {
    XML::Block X(filename.c_str());
    int Nline = X["NumberOfElements"].getInteger();
    nk = X["NumberOfInverseLattice"].getInteger();
    int nsites = Nline/nk;

    for (int i = 0; i < nsites; i++) {
      COSrk.push_back(std::vector<double>(nk));
      SINrk.push_back(std::vector<double>(nk));
    }

    int count = 0;
    for (int i = 0; i < X.NumberOfBlocks(); i++) {
      XML::Block& B = X[i];
      if (B.getName() == "SF") {
        double COSrk_ = B.getDouble(0);
        double SINrk_ = B.getDouble(1);
        int isite     = B.getInteger(2);
        int ksite     = B.getInteger(3);
        //cout<<isite<<"-"<<jsite<<endl;
        COSrk[isite][ksite] = COSrk_;
        SINrk[isite][ksite] = SINrk_;
        count++;
      }
    }

    if (count != Nline) {
      std::cout << "ERROR Nline( " << Nline << " ) != count( " << count << " )" << std::endl;
      exit(1);
    }
  }
};

#endif  // WAVEVECTOR_H
