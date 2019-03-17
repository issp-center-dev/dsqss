#ifndef WAVEVECTOR_HPP
#define WAVEVECTOR_HPP

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include "debug.hpp"
#include "accumulator.hpp"
#include "parameter.hpp"
#include "xml.h"

struct WaveVector {
  bool defined;
  int NSITES;
  int NK;

  std::vector<std::vector<double> > COSrk;
  std::vector<std::vector<double> > SINrk;

  WaveVector(Parameter const& param);
};

WaveVector::WaveVector(Parameter const& param) : defined(false), NSITES(0), NK(0)
{
  AutoDebugDump("WaveVector::WaveVector");
  if (param.WVFILE.length() > 0) {
    defined = true;
    XML::Block X(param.WVFILE.c_str());

    NSITES = X["NumberOfSites"].getInteger();
    NK = X["NumberOfWaveVectors"].getInteger();

    for (int i = 0; i < NSITES; i++) {
      COSrk.push_back(std::vector<double>(NK));
      SINrk.push_back(std::vector<double>(NK));
    }

    int count = 0;
    for (int i = 0; i < X.NumberOfBlocks(); i++) {
      XML::Block& B = X[i];
      if (B.getName() == "RK") {
        double COSrk_ = B.getDouble(0);
        double SINrk_ = B.getDouble(1);
        int site     = B.getInteger(2);
        int wave     = B.getInteger(3);
        COSrk[site][wave] = COSrk_;
        SINrk[site][wave] = SINrk_;
      }
    }
  }
};


#endif  // WaveVector_H
