#ifndef DLA_HPP
#define DLA_HPP

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/time.h>

#ifdef MULTI
#include "mpi.h"
#endif
int N_PROC;  // the total number of processors
int I_PROC;  // the processor id of the current processor

FILE* FERR = stdout;

#include "random.h"
Random RND;

#include "calctimer.hpp"

inline void abort(const char* s) {
  printf("%s\n", s);
  exit(0);
}

#ifdef DEB
bool DEBUG = true;
#else
bool DEBUG = false;
#endif

bool ALERT = false;

#include "algorithm.hpp"
#include "array.h"
#include "io.h"
#include "lattice.hpp"
#include "link.hpp"
#include "measure.hpp"
#include "sf.hpp"
#include "cf.hpp"
#include "ck.hpp"
#include "name.h"
#include "objects.hpp"
#include "parameter.hpp"

class Simulation {
private:
  int ISET;
  int IMCSE;
  int IMCSD;
  int IMCS;
  int ICYC;

  int ISETstart;
  int IMCSDstart;
  int IMCSstart;

  double AMP;       // the average mean path of the worm per cycle
  bool EndOfCycle;  // set to be true when the cycle ends

public:
  int np;
  Simulation(Parameter& P0);

  void calc();

  Parameter& P;
  Algorithm ALG;
  Lattice LAT;
  Measurement MSR;
  CalcTimer calctimer;
  SF sf;
  CF cf;
  CK ck;
  Worm W;

  Vertex V4REF;

  void reset_counters();
  void Set(int ntherm, int nmcs);
  void set_NCYC();

  void Sweep(bool thermalized);
  double Cycle(bool thermalized);
  bool PlaceWorm();
  double MoveHead(bool thermalized);

  double UP_ONESTEP(bool thermalized);
  double DOWN_ONESTEP(bool thermalized);

  double calculate_sign();

  double tail_tau;
  int tail_site;

  void Check();
  void dump(const char*);

private:
  bool isChainjob;
  bool isEnd;  //true->exit(0); false-> read cjob.bin
  std::string CJOBFILE;
  Timer cjob_timer;
  std::ofstream cjobout;
  std::ifstream cjobin;
  void BinaryIO();
  void load();
  void save();
  void end_cjob();
  void end_job();
};

#endif // DLA_HPP
