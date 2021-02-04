// DSQSS (Discrete Space Quantum Systems Solver)
// Copyright (C) 2018- The University of Tokyo
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SRC_DLA_DISPLACEMENT_HPP_
#define SRC_DLA_DISPLACEMENT_HPP_

#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

#include "accumulator.hpp"
#include "debug.hpp"
#include "parameter.hpp"
#include "xml.h"

struct Displacement {
  bool defined;
  int NSITES;
  int nkinds;

  std::vector<int> NR;                // the number of pairs with the same dR
  std::vector<std::vector<int> > IR;  // IR[isite][jsite] == disp_index

  explicit Displacement(Parameter const& param);
  void read(XML::Block const& X);
};

Displacement::Displacement(Parameter const& param)
    : defined(false), NSITES(0), nkinds(0) {
  AutoDebugDump("Displacement::Displacement");
  if (param.DISPFILE.length() > 0) {
    defined = true;
    XML::Block X(param.DISPFILE.c_str());

    NSITES = X["NumberOfSites"].getInteger();
    nkinds = X["NumberOfKinds"].getInteger();

    NR.resize(nkinds, 0);

    for (int i = 0; i < NSITES; i++) {
      IR.push_back(std::vector<int>(NSITES, nkinds));
    }

    for (int i = 0; i < X.NumberOfBlocks(); i++) {
      XML::Block& B = X[i];
      if (B.getName() == "R") {
        int kind = B.getInteger(0);
        int isite = B.getInteger(1);
        int jsite = B.getInteger(2);
        IR[isite][jsite] = kind;
        NR[kind]++;
      }
    }
  }
}

#endif  // SRC_DLA_DISPLACEMENT_HPP_
