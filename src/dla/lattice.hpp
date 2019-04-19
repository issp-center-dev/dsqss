#ifndef LATTICE_H
#define LATTICE_H

//######################################################################

#include <cmath>
#include <cstdio>
#include <exception>
#include <string>
#include <boost/math/special_functions/fpclassify.hpp>
#include "debug.hpp"
#include "io.h"
#include "objects.hpp"
#include "parameter.hpp"

//######################################################################

class Lattice {
public:
  class Edge {
  public:
    void init(int _d, int _a, int _b) {
      bd = _d;
      A  = _a;
      B  = _b;
    };

    int A;
    int B;
    int bd;
  };

private:
  Site* site;                // the list of the poiters to sites
  Interaction* interaction;  // the list of the pointers to interactions
  Edge* edge;                // the list of the poiters to sites

  XML::Block X;
  Algorithm& ALG;

public:
  int D;        // dimension
  int BD;       // bond dimension
  int* L;       // linear size
  double BETA;  // inverse temperature
  int NSITE;    // total number of sites
  int NINT;     // total number of interactions
  int NSTYPE;   // number of site types
  int NITYPE;   // number of interaction types
  int NEDGE;

  double** vec;

  Lattice(const char* FNAME, Algorithm& A);
  Lattice(Parameter const& P, Algorithm& A);

  ~Lattice();

  void read();

  void initialize();

  Site& S(int i) { return site[i]; };

  Interaction& I(int i) { return interaction[i]; };

  Edge& EDGE(int i) { return edge[i]; }

  int countVertices();

  void setBeta(double beta);

  void show_param(FILE* F) {
    fprintf(F, "P D       = %12d\n", D);
    fprintf(F, "P L       =      ");
    for (int i = 0; i < D; i++) {
      fprintf(F, " %6d", L[i]);
    }
    fprintf(F, "\n");
    fprintf(F, "P BETA    = %24.16f\n", BETA);
  };

  void dump();
};

//######################################################################

inline Lattice::Lattice(const char* FNAME, Algorithm& A) : ALG(A) {
  AutoDebugDump("Lattice::Lattice");
  X.initialize(FNAME, "LATTICE");
  read();
  initialize();
}

inline Lattice::Lattice(Parameter const& P, Algorithm& A) : ALG(A) {
  AutoDebugDump("Lattice::Lattice");
  X.initialize(P.LATFILE, "LATTICE");
  read();

  if(boost::math::isfinite(P.BETA)){
    setBeta(P.BETA);
  }

  initialize();
}

//======================================================================

void Lattice::read() {
  AutoDebugDump("Lattice::read");

  D = X["Dimension"].getInteger();
  L = new int[D];
  for (int i = 0; i < D; i++) {
    L[i] = X["LinearSize"].getInteger(i);
  }
  BETA = 1.0;

  NSITE  = X["NumberOfSites"].getInteger();
  NINT   = X["NumberOfInteractions"].getInteger();
  NSTYPE = X["NumberOfSiteTypes"].getInteger();
  NITYPE = X["NumberOfInteractionTypes"].getInteger();

  site        = new Site[NSITE];
  interaction = new Interaction[NINT];

  bool INIT_I = true;
  bool INIT_V = true;
  BD          = -1;
  edge        = NULL;
  vec         = NULL;

  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block& B = X[i];

    if (B.getName() == "S") {
      int id = B.getInteger(0);
      int st = B.getInteger(1);
      S(id).init(ALG.getSiteProperty(st));

      S(id).setBeta(BETA);
    }
    if (B.getName() == "I") {
      int id = B.getInteger(0);
      int it = B.getInteger(1);
      int nb = B.getInteger(2);
      I(id).init(ALG.getInteractionProperty(it));
      for (int ii = 0; ii < nb; ii++) {
        int sid = B.getInteger(3 + ii);
        I(id).setSite(ii, S(sid));
      }
      //#ifdef WINDING

      if (B.NumberOfValues() > 3 + nb) {
        if (INIT_I) {
          NEDGE  = X["NumberOfEdgeInteractions"].getInteger();
          edge   = new Edge[NEDGE];
          INIT_I = false;
        }

        int eid  = B.getInteger(3 + nb);
        int edim = B.getInteger(4 + nb);
        if (eid >= 0 && nb == 2) {
          EDGE(eid).init(edim, I(id).site(0).id() - 1, I(id).site(1).id() - 1);
        }
      }
      //#endif
    }
    if (B.getName() == "Direction") {
      if (INIT_V) {
        BD  = X["NumberOfBondDirections"].getInteger();
        vec = new double*[D];
        for (int di = 0; di < D; di++) {
          vec[di] = new double[BD];
        }

        INIT_V = false;
      }

      int bd = B.getInteger(0);
      for (int di = 0; di < D; di++) {
        double length = B.getDouble(1 + di);
        vec[di][bd]   = length;
      }
    }
  }

  if (DEBUG) dump();
}

//======================================================================
#include <set>
void Lattice::initialize() {
  AutoDebugDump("Lattice::initialize");

  //set<Interaction*> InteractionOnEachSite[NSITE];
  std::set<Interaction*>* InteractionOnEachSite;
  InteractionOnEachSite = new std::set<Interaction*>[NSITE];

  int NRVIMAX = 0;  // maximum number of registered vertex informations

  for (int iid = 0; iid < NINT; iid++) {
    for (int i = 0; i < I(iid).NBODY(); i++) {
      InteractionOnEachSite[I(iid).site(i).id() - 1].insert(&I(iid));
    }
  }

  for (int sid = 0; sid < NSITE; sid++) {
    S(sid).setNCI(InteractionOnEachSite[sid].size());

    int counter                    = 0;
    std::set<Interaction*>::iterator it = InteractionOnEachSite[sid].begin();

    while (it != InteractionOnEachSite[sid].end()) {
      S(sid).setCI(counter, (*it));
      ++it;
      ++counter;
    }

    InteractionOnEachSite[sid].clear();
    if (NRVIMAX < S(sid).getNCI()) NRVIMAX = S(sid).getNCI();
  }

  TheRVIPool.init(NRVIMAX * NRVIMAX);
  delete[] InteractionOnEachSite;
}

inline Lattice::~Lattice() {
  // printf("*** Destroying Lattice\n");
  if (L != 0) { delete[] L; }
  if (site != 0) { delete[] site; }
  if (interaction != 0) { delete[] interaction; }

  if (edge != NULL) { delete[] edge; }
  if (vec != NULL) {
    for (int i = 0; i < D; i++) {
      delete[] vec[i];
    }
    delete[] vec;
  }
}

inline int Lattice::countVertices() {
  int NV = 0;
  for (int b = 0; b < NINT; b++) {
    NV += I(b).count();
  }
  return NV;
}

inline void Lattice::dump() {
  printf("\n");
  printf("Lattice Information:\n");
  printf("  D      = %d\n", D);
  printf("  BD     = %d\n", BD);
  printf("  L      =");
  for (int i = 0; i < D; i++) {
    printf(" %d", L[i]);
  }
  printf("\n");
  printf("  BETA   = %24.16f\n", BETA);
  printf("  NSITE  = %d\n", NSITE);
  printf("  NINT   = %d\n", NINT);
  printf("  NSTYPE = %d\n", NSTYPE);
  printf("  NITYPE = %d\n", NITYPE);
  printf("  NEDGE = %d\n", NEDGE);
  printf("\n");
  for (int i = 0; i < NSITE; i++) {
    S(i).dump();
  }
  printf("\n");
  for (int i = 0; i < NINT; i++) {
    I(i).dump();
  }
}

void Lattice::setBeta(double beta) {
  if(beta < 0.0){
    std::string msg("ERROR: invalid BETA, ");
    msg += tostring(beta);
    util::ERROR(msg.c_str());
  }
  BETA = beta;
  for(int i=0;i<NSITE; ++i){
      S(i).setBeta(BETA);
  }
}

#endif
