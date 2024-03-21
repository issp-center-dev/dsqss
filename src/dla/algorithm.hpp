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

// ######################################################################
// ####
// ####  World-Line Monte Carlo simulation
// ####                       by the Directed-Loop Algorithm
// ####
// ####                                 Mar.03 / 2005, Naoki Kawashima
// ####
// ######################################################################

// ######################################################################
// ####
// ####  World-Line Monte Carlo simulation
// ####                       by the non-Vertex Directed-Loop Algorithm
// ####
// ####                                 Nov.11 / 2007, Yasuyuki Kato
// ####
// ######################################################################

#ifndef SRC_DLA_ALGORITHM_HPP_
#define SRC_DLA_ALGORITHM_HPP_

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "array.h"
#include "debug.hpp"
#include "io.h"
#include "link.hpp"
#include "name.h"
#include "xml.h"

class Algorithm;
class SiteProperty;
class SiteInitialConfiguration;
class InteractionProperty;
class VertexProperty;
class VertexInitialConfiguration;
class ScatteringChannel;

// ######################################################################
// ####  Object Property Declarations
// ######################################################################

class SiteInitialConfiguration {
 public:
  int State;
  int NCH;
  Array<ScatteringChannel> CH;
  SiteInitialConfiguration() { CH.setLabel("SiteInitialConfiguration::CH"); }
  int id() { return State; }
  void initialize(XML::Block& X);
  void dump();
};

class SiteProperty {
 public:
  int STYPE;
  int VTYPE;
  VertexProperty* _VP;
  int NX;
  std::vector<double> XVALS;
  int NIC;
  Array<SiteInitialConfiguration> IC;
  SiteProperty() : _VP(0) { IC.setLabel("SiteProperty::IC"); };

  int id() { return STYPE; }
  void initialize(XML::Block& X);
  void setVertexProperty(VertexProperty& VP) { _VP = &VP; }
  VertexProperty& getVertexProperty() { return *_VP; }
  SiteInitialConfiguration& getInitialConfiguration(int x) { return IC[x]; }
  void dump();
};

class ScatteringChannel {
 public:
  int OUT;
  int XOUT;
  double PROB;

  void initialize(XML::Block& X);
  void dump();
};

class InteractionProperty {
 public:
  int ITYPE;
  int VTYPE;
  int NBODY;
  int NXMAX;
  double EBASE;
  VertexProperty* _VP;
  Array<int> STYPE;
  Array<double> VertexDensity;
  Array<double> AverageInterval;
  Array<double> Sign;

  InteractionProperty() : _VP(0) {
    STYPE.setLabel("InteractionProperty::STYPE");
    VertexDensity.setLabel("InteractionProperty::VertexDensity");
    AverageInterval.setLabel("InteractionProperty::AverageInterval");
  };

  void setVertexProperty(VertexProperty& vp) { _VP = &vp; }
  VertexProperty& getVertexProperty() { return *_VP; }

  double sign(int* x) { return Sign(x); }
  double sign(std::vector<int> const& x) { return Sign(x); }

  int id() { return ITYPE; }
  void initialize(XML::Block& X);
  void dump();
};

class VertexProperty {
 public:
  int VTYPE;
  int VCAT;
  int NBODY;
  int NLEG;
  int NST;
  int NIC;
  int NHTYPE;
  int NXMAX;
  int MIC;
  Array<int> STYPE;
  Array<VertexInitialConfiguration> IC;
  Array<VertexInitialConfiguration*> _IC;
  Array<int> StateCode;
  Array<int> SCNK;  // State Code for NON-KINK

  VertexProperty() {
    STYPE.setLabel("VertexProperty::STYPE");
    IC.setLabel("VertexProperty::IC");
    _IC.setLabel("VertexProperty::_IC");
    StateCode.setLabel("VertexProperty::StateCode");
    SCNK.setLabel("VertexProperty::StateCode4NON-KINK");
  }

  int id() { return VTYPE; }

  void initialize(XML::Block& X, int i);

  int getSiteType(int out);
  bool isTerminal() { return VCAT == VCAT::TERM; }
  VertexInitialConfiguration& getIC(int st, int inc, int xinc);

  void dump();
};

class VertexInitialConfiguration {
 public:
  int ID;

  Array<ScatteringChannel> CH;

  VertexInitialConfiguration() : State(0) {
    ID = -1;
    CH.setLabel("VertexInitialConfiguration::CH");
  }

  ~VertexInitialConfiguration() {
    if (State != 0) delete[] State;
  }

  int NLEG;
  int* State;
  int INC;
  int XINC;
  int NCH;

  int MCH;      // max of NCH
  double dRHO;  // [vertex density] * [1-p1]
  void setID(int i) { ID = i; }
  int id() { return ID; }
  void initialize(XML::Block& X);

  void initialize();
  ScatteringChannel& getScatteringChannel();
  ScatteringChannel& getScatteringChannel(double drho);
  void dump();
};

class Algorithm {
 private:
  XML::Block X;
  Array<SiteProperty> SPROP;
  Array<InteractionProperty> IPROP;
  Array<VertexProperty> VPROP;

  int ix;

 public:
  double getBlock(const std::string& s, double i) {
    return X["General"][s].getDouble();
  }
  int getBlock(const std::string& s, int i) {
    return X["General"][s].getInteger();
  }

  explicit Algorithm(std::string const& FNAME);
  Algorithm() {}
  ~Algorithm();

  int NSTYPE;  // Number of site types
  int NITYPE;  // Number of interaction types
  int NVTYPE;  // Number of bond types
  int NXMAX;   // Maximum number of segment states

  // double WDIAG; // Artificial weight attached to the diagonal state
  // Used in relating the worm-head mean-path to the susceptibility

  void read();
  void initialize();

  int MXNIC1;  // Maximum number of Vertex InitialConfigurations
  void set_i(int n) { ix = n; }
  void set_alg(int n);

  SiteProperty& getSiteProperty(int i) { return SPROP[i]; }
  InteractionProperty& getInteractionProperty(int i) { return IPROP[i]; }
  VertexProperty& getVertexProperty(int i) { return VPROP[i]; }

  void dump() { X.dump(); }
};

// ######################################################################
// ###########  Member Functions  #######################################
// ######################################################################

inline Algorithm::Algorithm(std::string const& FNAME) {
  AutoDebugDump("Algorithm::Algorithm");

  SPROP.setLabel("Algorithm::SPROP");
  IPROP.setLabel("Algorithm::IPROP");
  VPROP.setLabel("Algorithm::VPROP");

  X.initialize(FNAME.c_str());

  int ixx = X["General"]["NXMAX"].getInteger();
  MXNIC1 = 24 * (ixx - 1) * (ixx - 1);

  read();
  initialize();
}

void Algorithm::set_alg(int ix) {
  AutoDebugDump("Algorithm::set_alg");

  SPROP.setLabel("Algorithm::SPROP");
  IPROP.setLabel("Algorithm::IPROP");
  VPROP.setLabel("Algorithm::VPROP");

  MXNIC1 = 0;

  std::stringstream ss;
  ss << ix << ".xml";
  X.initialize(ss.str().c_str());
  read();
  initialize();
}

void Algorithm::read() {
  AutoDebugDump("Algorithm::read");

  NSTYPE = X["General"]["NSTYPE"].getInteger();  // 1 []operator
  NITYPE = X["General"]["NITYPE"].getInteger();  // 1
  NVTYPE = X["General"]["NVTYPE"].getInteger();  // 2
  NXMAX = X["General"]["NXMAX"].getInteger();    // 2
  // WDIAG  = X["General"]["WDIAG" ].getDouble(); // 0.25

  SPROP.init(1, NSTYPE);  // 1,SiteProperty
  IPROP.init(1, NITYPE);  // 1,InteractionProperty
  VPROP.init(1, NVTYPE);  // 2,VertexProperty

  for (int i = 0; i < NITYPE; i++) {  // 1 IPROP(i) : return val[i]
    IPROP(i).NXMAX = NXMAX;
  }

  for (int i = 0; i < NVTYPE; i++) {  // 2
    VPROP(i).NXMAX = NXMAX;
  }

  for (int i = 0; i < X.NumberOfBlocks(); i++) {  // X.numberofblocks=6
    XML::Block& B = X[i];
    const std::string& name = B.getName();

    if (name == "Site") {
      int id = B["STYPE"].getInteger();  // id=0
      // SitePropertyClass setting is finished at this point
      // note: SPROP(0) returns SiteProperty& val[0]
      SPROP(id).initialize(B);
    }

    if (name == "Interaction") {
      int id = B["ITYPE"].getInteger();  // id=0
      IPROP(id).initialize(B);
    }

    if (name == "Vertex") {
      int id = B["VTYPE"].getInteger();  // id=0,1
      VPROP(id).initialize(B, MXNIC1);
    }
  }
}

void Algorithm::initialize() {
  AutoDebugDump("Algorithm::initialize");

  // Site::_VP
  for (int i = 0; i < NSTYPE; i++) {        // 1
    int vt = SPROP(i).VTYPE;                // 2
    SPROP(i).setVertexProperty(VPROP(vt));  // VPROP(vt) {return val}
  }

  // scattering prob
  for (int i = 0; i < NSTYPE; i++) {  // 1
    SiteProperty& SP = SPROP(i);
    for (int j = 0; j < SP.NIC; j++) {          // NIC=NX=Number of states 2
      SiteInitialConfiguration& IC = SP.IC[j];  // operator return val[i]
      double p = 0.0;

      for (int k = 0; k < IC.NCH; k++) {  // number of channnels
        ScatteringChannel& SC = IC.CH[k];
        p += SC.PROB;
        SC.PROB = p;
      }
      if (fabs(p - 1.0) > EPS) {  // EPS=1.d-14
        printf("Algorithm::initialize> Error.\n");
        printf("  The sum of probabilities is not equal to 1.\n");
        SP.dump();
        IC.dump();
        exit(0);
      }
      IC.CH[IC.NCH - 1].PROB = 1.0;
    }
  }

  // Sitetype of vertices
  for (int i = 0; i < NSTYPE; i++) {
    SiteProperty& S = SPROP(i);
    VertexProperty& V = VPROP(S.VTYPE);

    int NLEG = 2;
    for (int l = 0; l < NLEG; l++) {
      V.STYPE[l] = S.STYPE;
    }
  }
  for (int i = 0; i < NITYPE; i++) {
    InteractionProperty& I = IPROP(i);
    VertexProperty& V = VPROP(I.VTYPE);
    int NLEG = 2 * I.NBODY;
    for (int l = 0; l < NLEG; l++) {
      V.STYPE[l] = I.STYPE[l / 2];
    }
  }

  // _IC of VertexProperty

  for (int i = 0; i < NVTYPE; i++) {
    VertexProperty& VP = VPROP(i);

    int nst = 0;

    for (int ic = 0; ic < VP.NIC; ic++) {
      VertexInitialConfiguration& IC = VP.IC[ic];

      int nl = VP.NLEG;
      int* x = IC.State;
      int inc = IC.INC;
      int xinc = IC.XINC;
      int st;
      bool isKink = false;
      std::vector<int> xx(nl / 2);

      for (int il = 0; il < nl; il += 2) {
        if (x[il] != x[il + 1])
          isKink = true;
        else
          xx[il / 2] = x[il];
      }

      if (VP.StateCode(x) == STATE::UNDEF) {  // operator return val(ID(x))
        VP.StateCode(x) = nst;
        st = nst;
        if (!isKink) VP.SCNK(xx) = nst;
        nst++;
      } else {
        st = VP.StateCode(x);
      }

      VP._IC(st, inc, xinc) = &IC;  // generate INDEX using st,inc,xinc
    }                               // end VP.NICloop
  }

  // _VP of InteractionProperty
  for (int i = 0; i < NITYPE; i++) {
    InteractionProperty& I = IPROP(i);
    I.setVertexProperty(VPROP(I.VTYPE));
  }

  // cumprod scattering rate and re-re-definition of ScatteringChannel::PROB
  // Vertex representing NOT KINK has PROB = [vertexdensity]*[probablity]
  for (int i = 0; i < NVTYPE; i++) {
    VertexProperty& VP = VPROP(i);

    for (int j = 0; j < VP.NIC; j++) {
      VertexInitialConfiguration& IC = VP.IC[j];

      bool isKink = false;
      std::vector<int> x(VP.NBODY);
      for (int ileg = 0; ileg < IC.NLEG; ileg += 2) {
        if (IC.State[ileg] != IC.State[ileg + 1]) {
          isKink = true;
        }
        x[ileg / 2] = IC.State[ileg];
      }
      double p = 0.0;

      bool isVertex = false;  // false: kink or term, true: means interaction
      int i_type = 0;
      for (i_type = 0; i_type < NITYPE; i_type++) {
        if (IPROP[i_type].VTYPE == i) {
          isVertex = true;
          break;
        }
      }  // If VTYPE is for Vertex (not for term or tail), there should be
         // InteractionProperty with density of vertex.

      if (isKink || (!isVertex)) {
        IC.dRHO = 1.0;
        for (int k = 0; k < IC.NCH; k++) {
          ScatteringChannel& SC = IC.CH[k];
          p += SC.PROB;
          SC.PROB = p;
        }

      } else {
        double Vdensity = IPROP[i_type].VertexDensity(x);
        int count_ch = 0;
        double dR = Vdensity;

        for (int k = 0; k < IC.NCH; k++) {
          ScatteringChannel& SC = IC.CH[k];
          if (SC.OUT == IC.INC + 1 - 2 * (IC.INC % 2)) {
            dR = (1.0 - SC.PROB) * Vdensity;
          } else {
            p += (SC.PROB * Vdensity);
            IC.CH[count_ch] = SC;
            IC.CH[count_ch].PROB = p;
            count_ch++;
          }
        }
        IC.dRHO = dR;
        IC.NCH = count_ch;
      }
      if (fabs(p - IC.dRHO) > EPS * 10.0) {
        printf("Algorithm::initialize> Error.\n");
        printf("  The sum of probabilities is not equal to dRHO.\n");
        VP.dump();
        IC.dump();
        exit(0);
      }

      if (IC.NCH != 0) {
        IC.CH[IC.NCH - 1].PROB = IC.dRHO + EPS;
      } else {
        IC.dRHO = 0.0;
      }
    }
  }
}

inline Algorithm::~Algorithm() {}

void SiteProperty::initialize(XML::Block& X) {  // <Site>
  AutoDebugDump("SiteProperty::initialize");

  STYPE = X["STYPE"].getInteger();
  NX = X["NumberOfStates"].getInteger();
  VTYPE = X["VertexTypeOfSource"].getInteger();
  NIC = NX;

  IC.init(1, NIC);

  XVALS.resize(NX);
  for (int i = 0; i < NX; ++i) {
    XVALS[i] = X["LocalStates"].getDouble(i);
  }

  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block& B = X[i];
    if (B.getName() == "InitialConfiguration") {
      int st = B["State"].getInteger();
      IC[st].initialize(B);
    }
  }
}

void SiteProperty::dump() {
  AutoDebugDump("SiteProperty::dump");
  printf("\n");
  printf("<SiteProperty>\n");
  printf("  STYPE= %d\n", STYPE);
  printf("  NX=    %d\n", NX);
  printf("  VTYPE= %d\n", VTYPE);
  printf("  NIC=   %d\n", NIC);
  for (int i = 0; i < NIC; i++) IC[i].dump();
  printf("</SiteProperty>\n");
}

void SiteInitialConfiguration::initialize(XML::Block& X) {
  AutoDebugDump("SiteInitialConfiguration::initialize");

  State = X["State"].getInteger();
  NCH = X["NumberOfChannels"].getInteger();
  CH.init("SCH", 1, NCH);

  int ch = 0;
  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block& B = X[i];
    if (B.getName() == "Channel") CH[ch++].initialize(B);
  }

  if (ch != NCH) {
    printf("SiteInitialConfiguration::initialize> Error.\n");
    printf("  The actual number of channels (=%d)\n", ch);
    printf("  does not agree with NCH (=%d)\n", NCH);
  }
}

void SiteInitialConfiguration::dump() {
  AutoDebugDump("SiteInitialConfiguration::dump");
  printf("\n");
  printf("<SiteInitialConfiguration>\n");
  printf("  State= %d\n", State);
  printf("  NCH=   %d\n", NCH);
  for (int i = 0; i < NCH; i++) CH[i].dump();
  printf("</SiteInitialConfiguration>\n");
}

void ScatteringChannel::initialize(XML::Block& X) {
  OUT = X.getInteger(0);
  XOUT = X.getInteger(1);
  PROB = X.getDouble(2);
}

void ScatteringChannel::dump() {
  AutoDebugDump("ScatteringChannel::dump");
  //  printf("    CH= %2d", Code);
  printf("  OUT= %2d", OUT);
  printf(", XOUT= %2d", XOUT);
  printf(", P= %8.3f", PROB);
  printf("\n");
}

void InteractionProperty::initialize(XML::Block& X) {
  AutoDebugDump("InteractionProperty::initialize");
  ITYPE = X["ITYPE"].getInteger();
  VTYPE = X["VTYPE"].getInteger();
  NBODY = X["NBODY"].getInteger();
  EBASE = X["EBASE"].getDouble();
  STYPE.init(1, NBODY);
  STYPE.set_all(STYPE::UNDEF);
  VertexDensity.init(NBODY, NXMAX, ARRAY::EOL);
  VertexDensity.set_all(0.0);
  AverageInterval.init(NBODY, NXMAX, ARRAY::EOL);
  AverageInterval.set_all(-1.0);
  Sign.init(2 * NBODY, NXMAX, ARRAY::EOL);
  Sign.set_all(1.0);

  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block& B = X[i];
    if (B.getName() == "VertexDensity") {
      std::vector<int> x(NBODY);
      for (int ii = 0; ii < NBODY; ii++) {
        x[ii] = B.getInteger(ii);
      }
      double d = B.getDouble(NBODY);
      // VertexDensity   val[n] = d    (n:1-n)
      // AverageInterval val[n] = 1/d  (n:1-n)
      VertexDensity(x) = d;
      AverageInterval(x) = 1.0 / d;
    }
    if (B.getName() == "Sign") {
      std::vector<int> x(2 * NBODY);
      for (int ii = 0; ii < 2 * NBODY; ii++) {
        x[ii] = B.getInteger(ii);
      }
      double sgn = B.getDouble(2 * NBODY);
      Sign(x) = sgn;
    }
  }
}

inline void InteractionProperty::dump() {
  AutoDebugDump("InteractionProperty::dump");
  printf("\n");
  printf("<InteractionProperty>\n");
  printf("  ITYPE= %2d", ITYPE);
  printf(", VTYPE= %2d", VTYPE);
  printf(", NBODY= %2d", NBODY);
  printf(", NXMAX= %2d", NXMAX);
  printf(", EBASE= %24.16f\n", EBASE);
  IndexSystem& I = VertexDensity.index_system();
  std::vector<int> x(I.dimension());

  for (int i = 0; i < I.size(); i++) {
    I.coord(i, &(x[0]));
    printf("     (");
    for (int j = 0; j < I.dimension(); j++) {
      printf(" %1d", x[j]);
    }
    printf(") --> %8.3f, %8.3f\n", VertexDensity(x), AverageInterval(x));
  }
  printf("</InteractionProperty>\n");
}

void VertexProperty::initialize(XML::Block& X, int mx) {
  AutoDebugDump("VertexProperty::initialize");
  VTYPE = X["VTYPE"].getInteger();
  VCAT = X["VCATEGORY"].getInteger();
  NBODY = X["NBODY"].getInteger();
  NIC = X["NumberOfInitialConfigurations"].getInteger();
  NLEG = 2 * NBODY;

  STYPE.init(1, NLEG);
  STYPE.set_all(STYPE::UNDEF);

  StateCode.init(NLEG, NXMAX, ARRAY::EOL);
  StateCode.set_all(STATE::UNDEF);

  SCNK.init(NBODY, NXMAX, ARRAY::EOL);
  SCNK.set_all(STATE::UNDEF);

  NST = StateCode.size();
  _IC.init(3, NST, NLEG, NXMAX);
  _IC.set_all(0);

  int id = X["VTYPE"].getInteger();

  if (id == 0) {
    IC.init(1, NIC);
  } else {
    MIC = NIC;

    if (mx > NIC) MIC = mx;
    IC.init(1, MIC);
  }

  int ic = 0;
  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block& B = X[i];
    if (B.getName() == "InitialConfiguration") {
      IC[ic].setID(ic);
      IC[ic].NLEG = NLEG;
      IC[ic].initialize(B);
      ic++;
    }
  }

  if (id == 1) {
    for (int i = NIC; i < MIC; i++) {
      IC[i].setID(i);
      IC[i].NLEG = NLEG;
      IC[i].initialize();
    }
  }
}

inline int VertexProperty::getSiteType(int out) {
#ifdef DEB
  if (out < 0 || out >= NLEG) {
    printf("VertexProperty::getSiteType> Error.\n");
    printf("  The argument (= %d) is out of the bounds.\n", out);
    dump();
    exit(0);
  }
#endif
  return STYPE[out];
}

inline VertexInitialConfiguration& VertexProperty::getIC(int st, int inc,
                                                         int xinc) {
  return *_IC(st, inc, xinc);
}

void VertexProperty::dump() {
  AutoDebugDump("VertexProperty::dump");
  printf("\n");
  printf("<VertexProperty>\n");
  printf("  VTYPE  = %d\n", VTYPE);
  printf("  NBODY  = %d\n", NBODY);
  printf("  NLEG   = %d\n", NLEG);
  printf("  NST    = %d\n", NST);
  printf("  NIC    = %d\n", NIC);
  printf("  NXMAX  = %d\n", NXMAX);
  printf("  STYPE  = ");
  for (int i = 0; i < NLEG; i++) printf(" %d", STYPE[i]);
  printf("\n");
  std::vector<int> x(NLEG);
  IndexSystem& I = StateCode.index_system();
  for (int i = 0; i < I.size(); i++) {
    I.coord(i, &(x)[0]);
    int sc = StateCode[i];
    printf("  StateCode");
    for (int j = 0; j < I.dimension(); j++) {
      printf("[%1d]", x[j]);
    }
    printf(" = %d\n", sc);
  }
  IndexSystem& I2 = SCNK.index_system();
  for (int i = 0; i < I2.size(); i++) {
    I2.coord(i, &(x[0]));
    int sc = SCNK[i];
    printf("  StateCode4NON-KINK");
    for (int j = 0; j < I2.dimension(); j++) {
      printf("[%1d]", x[j]);
    }
    printf(" = %d\n", sc);
  }
  int y[3];
  IndexSystem& J = _IC.index_system();
  std::cout << "J.size() = " << J.size() << std::endl;
  for (int i = 0; i < J.size(); i++) {
    J.coord(i, y);
    if (_IC[i] != 0) {
      int icc = (*_IC[i]).id();
      printf("  (_IC[%1d][%1d][%1d])->id() = %d\n", y[0], y[1], y[2], icc);
    }
  }
  for (int i = 0; i < NIC; i++) IC[i].dump();
  printf("</VertexProperty>\n");
}

void VertexInitialConfiguration::initialize(XML::Block& X) {  // <initialconfig>
  AutoDebugDump("VertexInitialConfiguration::initialize");

  if (NLEG == 0) {
    printf("VertexInitialConfiguration::read> Error.\n");
    printf(" ...  NLEG has not been set.\n");
    exit(0);
  }

  State = new int[NLEG];
  for (int i = 0; i < NLEG; i++) {
    State[i] = X["State"].getInteger(i);
  }

  INC = X["IncomingDirection"].getInteger();
  XINC = X["NewState"].getInteger();
  NCH = X["NumberOfChannels"].getInteger();

  MCH = 4;
  CH.init("VCH", 1, MCH);

  int ch = 0;
  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block& B = X[i];
    if (B.getName() == "Channel") CH[ch++].initialize(B);
  }

  if (ch != NCH) {
    printf("SiteInitialConfiguration::initialize> Error.\n");
    printf("  The actual number of channels (=%d)\n", ch);
    printf("  does not agree with NCH (=%d)\n", NCH);
  }
}

void VertexInitialConfiguration::initialize() {  // <initialconfig>
  AutoDebugDump("VertexInitialConfiguration::initialize");

  if (NLEG == 0) {
    printf("VertexInitialConfiguration::read> Error.\n");
    printf(" ...  NLEG has not been set.\n");
    exit(0);
  }

  State = new int[NLEG];

  NCH = 4;  // X["NumberOfChannels"].getInteger();
  CH.init("VCH", 1, NCH);
}

inline ScatteringChannel& VertexInitialConfiguration::getScatteringChannel() {
  double p;
  if (NCH == 1) return CH[0];
  int ch;
  double r = RND.Uniform();
  for (ch = 0; ch < NCH; ch++) {
    p = CH[ch].PROB;
    if (r < p) break;
  }
  if (ch == NCH) {
    printf("ERROR! at Usual Vertex (KINK) \n");
    dump();
    printf("VertexInitialConfiguration::getScatteringChannel> Error.\n");
    printf("  ... ch=%d exceeds the limit.\n", ch);
    exit(0);
  }
  return CH[ch];
}

inline ScatteringChannel& VertexInitialConfiguration::getScatteringChannel(
    double drho) {
  if (NCH == 1) return CH[0];
  int ch;
  double p;
  for (ch = 0; ch < NCH; ch++) {
    p = CH[ch].PROB;
    if (drho < p) break;
  }
  if (ch == NCH) {
    printf("ERROR! at place Vertex (non-KINK)\n");
    dump();
    printf("VertexInitialConfiguration::getScatteringChannel> Error.\n");
    printf("  ... ch=%d exceeds the limit.\n", ch);
    exit(0);
  }
  return CH[ch];
}

void VertexInitialConfiguration::dump() {
  AutoDebugDump("VertexInitialConfiguration::dump");
  printf("\n");
  printf("<VertexInitialConfiguration> ID= %d\n", ID);
  printf("  NLEG= %d", NLEG);
  printf(", INC= %d", INC);
  printf(", XINC= %d", XINC);
  printf(", NCH= %d", NCH);
  printf(", dRHO= %f", dRHO);

  printf(", X= (");
  for (int i = 0; i < NLEG; i++) printf(" %1d", State[i]);
  printf(")\n");
  for (int i = 0; i < NCH; i++) CH[i].dump();
  printf("</VertexInitialConfiguration>\n");
}

#endif  // SRC_DLA_ALGORITHM_HPP_
