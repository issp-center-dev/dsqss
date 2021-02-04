//######################################################################
//####
//####  World-Line Monte Carlo simulation
//####                       by the Directed-Loop Algorithm
//####
//####                                 Mar.03 / 2005, Naoki Kawashima
//####
//######################################################################

//######################################################################
//####
//####  World-Line Monte Carlo simulation
//####                       by the non-Vertex Directed-Loop Algorithm
//####
//####                                 Nov.11 / 2007, Yasuyuki Kato
//####
//######################################################################

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

#ifndef DLA_HPP
#define DLA_HPP

#include <sys/time.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

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
#include "cf.hpp"
#include "ck.hpp"
#include "displacement.hpp"
#include "io.h"
#include "lattice.hpp"
#include "link.hpp"
#include "measure.hpp"
#include "name.h"
#include "objects.hpp"
#include "parameter.hpp"
#include "sf.hpp"
#include "wavevector.hpp"

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
  WaveVector WV;
  Displacement DISP;
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
  bool isEnd;  // true->exit(0); false-> read cjob.bin
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

#endif  // DLA_HPP
