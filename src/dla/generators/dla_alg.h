//
//     Algorithm File Generator for dla.cc
//
//                   Copy Right 2005, Naoki Kawashima
//

#include <cmath>
#include <vector>
#include "array.h"
#include "io.h"
#include "name.h"
#include "xml.h"

class InitialConfiguration;
class InitialConfigurationGroup;
class GENERAL;
class SITE;
class SOURCE;
class INTERACTION;
class VERTEX;
class QUANTITY;

//######################################################################

void SolveWeightEquation(int N, Array<double>& V, Array<double>& W);
bool testKink(int ist);
bool testForbidden(int ist);
void bubble_sort(int N, Array<double>& V, Array<int>& I);

//######################################################################

class GENERAL {
public:
  std::string comment;
  // "WeightDiagonal"
  // the artificial weight attached to the diagonal state
  // in solving the weight equation for the worm creation/annihilation
  // ( the same as 1/(N_{box} \eta^2) )
  double WeightDiagonal;
  GENERAL(XML::Block& X);
  void write();
};

//######################################################################

class SITE {
public:
  int ID;
  int TTYPE;  // the SOURCE type of the worm tail
  int VTYPE;  // the VERTEX type of the worm tail
  int NX;
  int* NumberOfChannels;
  SOURCE* _T;
  SOURCE& T() { return *_T; };
  VERTEX& V();
  Array<int> WormCreationNewState;
  Array<int> WormCreationDirection;
  Array<double> WormCreationProbability;

  SITE() {
    _T               = 0;
    ID               = STYPE::UNDEF;
    TTYPE            = TTYPE::UNDEF;
    VTYPE            = VTYPE::UNDEF;
    NX               = 0;
    NumberOfChannels = 0;
  };

  ~SITE() {
    if (NumberOfChannels != 0) delete[] NumberOfChannels;
    WormCreationNewState.reset();
    WormCreationDirection.reset();
    WormCreationProbability.reset();
  };

  void load(XML::Block& X);
  void SetInitialHeadTypeProbability();
  void write();
  void dump();
};

//######################################################################

class VERTEX {
private:
  IndexSystem* _INDX;

public:
  int ID;
  std::vector<InitialConfigurationGroup*> icg;
  InitialConfigurationGroup& ICG(int i) { return *(icg[i]); };
  void add_ICG(InitialConfigurationGroup& x) { icg.push_back(&x); };

  int NBODY;  // the number of sites interacting
  int NLEG;
  int NICG;  // number of initial configuration group
  int NICV;  // total number of initial configurations
  int NST;  // the number of initial states (not including worm type or direction)
  int CATEGORY;  // =0 (TERM), =1 (WORM), =2 (INTERACTION)
  double EBASE;
  Array<double> Weight;
  Array<int> SiteTypeOfLeg;
  Array<int> checked;

  void setINDX(IndexSystem& I) { _INDX = &I; };
  IndexSystem& INDX() { return *_INDX; };

  VERTEX() {
    ID       = VTYPE::UNDEF;
    NBODY    = 0;
    NLEG     = 0;
    NICG     = 0;
    NICV     = 0;
    NST      = 0;
    CATEGORY = VCAT::UNDEF;
    EBASE    = 0.0;
  };

  ~VERTEX(){};

  bool isTerm() { return CATEGORY == VCAT::TERM; };
  bool isWorm() { return CATEGORY == VCAT::WORM; };
  bool isInteraction() { return CATEGORY == VCAT::INT; };
  void Grouping();
  double ComputeEBASE();
  void ComputeScatteringProbability();
  void dump();
  void load(XML::Block& X);
  void write();
  int NumberOfValidInitialConfigurations();
  bool testKink(int ist);
  bool testForbidden(int ist);
};

//######################################################################

class SOURCE {
public:
  int ID;
  int STYPE;
  int VTYPE;
  VERTEX* _V;
  SOURCE() {
    ID    = TTYPE::UNDEF;
    STYPE = STYPE::UNDEF;
    VTYPE = VTYPE::UNDEF;
    _V    = 0;
  };
  VERTEX& V() {
    if (_V == 0) {
      printf("SOURCE::V> Error. _V is not defined.\n");
      exit(0);
    }
    return *_V;
  };
  double Weight(int x0, int x1) { return V().Weight(x0, x1); };
  void load(XML::Block& X);
  void dump();
};

//######################################################################

class INTERACTION {
public:
  int ID;
  int VTYPE;
  int NBODY;
  VERTEX* _V;
  VERTEX& V() { return *_V; };
  Array<double> VertexDensity;
  Array<double> Sign;

  INTERACTION() {
    ID    = ITYPE::UNDEF;
    VTYPE = VTYPE::UNDEF;
    NBODY = 0;
  };
  ~INTERACTION() { VertexDensity.reset(); };
  int STYPE(int i);
  void SetVertexDensity();
  void load(XML::Block& X);
  void write();
  void dump();
};

//######################################################################

class InitialConfiguration {
private:
  static int LastID;
  int ID;

public:
  friend class InitialConfigurationGroup;

  int NBODY;
  int NLEG;
  int NCH;
  int STI;
  int XINC;
  int INC;

  int* State;
  int* FinalState;
  int* FinalDirection;
  double* ScatteringProbability;
  VERTEX* _V;

  InitialConfiguration() {
    ID = LastID;
    LastID++;
    State                 = 0;
    FinalState            = 0;
    FinalDirection        = 0;
    ScatteringProbability = 0;
  };

  ~InitialConfiguration() {
    if (State != 0) delete[] State;
    if (ScatteringProbability != 0) delete[] ScatteringProbability;
    if (FinalState != 0) delete[] FinalState;
    if (FinalDirection != 0) delete[] FinalDirection;
  };

  bool isValid();
  void setV(VERTEX& V0) { _V = &V0; };
  VERTEX& V() { return *_V; };
  void init(VERTEX& V, int sti, int inc, int xinc, int NICmax);
  bool isKink();
  int& id() { return ID; }
  double weight();
  double vertex_weight();
  double worm_weight();
  void dump();
  void write();
};

int InitialConfiguration::LastID = 0;

//######################################################################

class InitialConfigurationGroup {
private:
  static int LastID;
  int ID;

public:
  int NIC;
  int NDIAG;
  Array<double> U;
  //  Array<double> W;
  //  Array<double> P;
  VERTEX* _V;
  Array<InitialConfiguration> IC;
  int NBODY;
  int NLEG;

  InitialConfigurationGroup() {
    ID = LastID;
    LastID++;
    _V = 0;
  };

  ~InitialConfigurationGroup() { IC.reset(); };

  void setV(VERTEX& V0) { _V = &V0; };

  VERTEX& V() {
    if (_V == 0) {
      printf("VERTEX::V> Error. _V has not been defined.\n");
      exit(0);
    }
    return *_V;
  };

  int NumberOfValidInitialConfigurations();

  //  void init_w( int nbody , int sti );
  void init(VERTEX& v, int nbody, int sti, int inc, int xinc);
  int number_of_diagonal_states();
  int number_of_offdiagonal_states();
  double minimum_diagonal_weight();
  double maximum_diagonal_weight();
  double maximum_offdiagonal_weight();
  double sum_of_all_weights();
  double ebase();  // the maximum base energy to make
                   // all diagonal weights positive
  void add_to_diagonal_element(double E);
  void numbering(int& ICNF);
  void ScatteringProbability();
  void ResetWeight();

  void dump();
  void write();
};

int InitialConfigurationGroup::LastID = 0;

//######################################################################

class QUANTITY {
private:
  int ID;
  std::string NAME;

public:
  Array<double> Value;
  Array<bool> isDefined;
  int getID() { return ID; };
  void setID(int i) { ID = i; };
  const std::string& getName() { return NAME; };
  void setName(const std::string& s) { NAME = s; };
  void load(XML::Block& X);
  //  void dump();
  void write();
};

//######################################################################

int INTERACTION::STYPE(int i) { return _V->SiteTypeOfLeg(2 * i); }

//######################################################################

VERTEX& SITE::V() {
  if (_T == 0) {
    printf("SITE::V> Error. _T is not defined.\n");
    exit(0);
  }
  return T().V();
}

//######################################################################

const char* HFILE;  // file name of the hamiltonian and the worm
const char* AFILE;  // file name of algorithm data file
FileReader H;
FILE* FALG;

SITE* Site;
SOURCE* Source;
INTERACTION* Interaction;
VERTEX* Vertex;
QUANTITY* Quantity;

bool isKink(int st);

int NVTYPE;
int NSTYPE;  // the number of site types
int NITYPE;  // the number of interaction types
int NHTYPE;  // the number of worm head types
int NQTYPE;  // the number of observable types
int NXMAX;

bool MON = false;  // true for monitoring
