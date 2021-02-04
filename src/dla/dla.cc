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

#include <cmath>
#include <string>
#include <vector>

#include "../common/version.h"
#include "chainjob.hpp"
#include "debug.hpp"
#include "util.hpp"

#include "dla.hpp"

//######################################################################

int main(int argc, char* argv[]) {
  AutoDebugDump("main");
  // MPI  Initialization & Getting Size and Rank
#ifdef MULTI
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &N_PROC);
  MPI_Comm_rank(MPI_COMM_WORLD, &I_PROC);

  if (I_PROC == 0) {
    printf(">>> The program is being run with MPI mode.( N_PROC = %d ) \n\n",
           N_PROC);
  }
#else
  N_PROC = 1;
  I_PROC = 0;
  printf(">>> The program is being run on serial mode. \n\n");
#endif

#ifdef DEB
  printf("\n\n>>> The program is being run on DEBUG mode.\n\n\n");
#endif

  Parameter P(argc, argv);

  TheSegmentPool.init(P.NSEGMAX);
  TheVertexPool.init(P.NVERMAX);

  RND.setSeed(P.SEED, 32);

  Simulation Sim(P);

#ifdef MULTI
  MPI_Finalize();
#endif
}

Simulation::Simulation(Parameter& P0)
    : P(P0),
      ALG(P.ALGFILE),
      LAT(P, ALG),
      WV(P),
      DISP(P),
      MSR(P, LAT, ALG, WV),
      calctimer(P.NMCS),
      sf(P, LAT, ALG, WV),
      cf(P, LAT, ALG, DISP),
      ck(P, LAT, ALG, WV) {
  AutoDebugDump("Simulation::Simulation");

  LAT.setBeta(P.BETA);

  reset_counters();

  if (P.SIMTIME > 0.0) {
    BinaryIO();
  } else {
    isChainjob = false;
  }
  if (!isChainjob) {
    if (I_PROC == 0) {
      std::cout << "Determining hyperparameter NCYC : ";
      set_NCYC();
    }
#ifdef MULTI
    int ncyc = P.NCYC;
    MPI_Bcast(&ncyc, 1, MPI_INT, 0, MPI_COMM_WORLD);
    P.NCYC = ncyc;
#endif
    if (I_PROC == 0) {
      std::cout << P.NCYC << std::endl;
    }
  } else {
    std::cout << "Reading checkpoint file takes " << cjob_timer.elapsed()
              << " sec." << std::endl;
  }
  isEnd = false;

  if (I_PROC == 0) {
    std::cout << "Start main calculation." << std::endl;
  }

  Timer timer;

  for (ISET = ISETstart; ISET < P.NSET; ISET++) {
    Set((ISET == 0 ? P.NTHERM : P.NDECOR), P.NMCS);
    double elapsed = timer.elapsed();
    double ETR = elapsed * (P.NSET - ISET - 1) / (ISET - ISETstart + 1);
    if (I_PROC == 0) {
      std::cout << ISET + 1 << " / " << P.NSET << " done. [Elapsed: " << elapsed
                << " sec. ETR: " << ETR << " sec.]" << std::endl;
    }
  }
  ISETstart = 0;

#ifdef MULTI
  MSR.allreduce(MPI_COMM_WORLD);
  calctimer.allreduce(MPI_COMM_WORLD);
  sf.allreduce(MPI_COMM_WORLD);
  cf.allreduce(MPI_COMM_WORLD);
  ck.allreduce(MPI_COMM_WORLD);
#endif
  MSR.summary();
  calctimer.summary();
  sf.summary();
  cf.summary();
  ck.summary();

  if (I_PROC == 0) {
    P.openfile();
    fprintf(P.FOUT, "C This is DSQSS ver.%s\n\n", DSQSS_VERSION);
    fprintf(P.FOUT, "I N_PROC = %d\n", N_PROC);
    LAT.show_param(P.FOUT);
    P.dump(P.FOUT);
    MSR.show(P.FOUT);
    calctimer.show(P.FOUT);

    int ns_used = TheSegmentPool.number_of_used_elements();
    int nv_used = TheVertexPool.number_of_used_elements();
    int nrvi_used = TheRVIPool.number_of_used_elements();
    fprintf(P.FOUT, "I [the maximum number of segments]          = %d\n",
            ns_used);
    fprintf(P.FOUT, "I [the maximum number of vertices]          = %d\n",
            nv_used);
    fprintf(P.FOUT, "I [the maximum number of reg. vertex info.] = %d\n",
            nrvi_used);

    if (P.FOUT4CF) {
      cf.summary();
      fprintf(P.FOUT4CF, "C This is dla.cc ver.%s\n\n", DSQSS_VERSION);
      fprintf(P.FOUT4CF, "I N_PROC = %d\n", N_PROC);
      LAT.show_param(P.FOUT4CF);
      P.dump(P.FOUT4CF);
      cf.show(P.FOUT4CF);
    }

    if (P.FOUT4SF) {
      fprintf(P.FOUT4SF, "C This is dla.cc ver.%s\n\n", DSQSS_VERSION);
      fprintf(P.FOUT4SF, "I N_PROC = %d\n", N_PROC);
      LAT.show_param(P.FOUT4SF);
      P.dump(P.FOUT4SF);
      sf.show(P.FOUT4SF);
    }

    if (P.FOUT4CK) {
      fprintf(P.FOUT4CK, "C This is dla.cc ver.%s\n\n", DSQSS_VERSION);
      fprintf(P.FOUT4CK, "I N_PROC = %d\n", N_PROC);
      LAT.show_param(P.FOUT4CK);
      P.dump(P.FOUT4CK);
      ck.show(P.FOUT4CK);
    }
    P.closefile();
  }

  if (P.SIMTIME > 0.0) {
    isEnd = true;
    cjobout.open(CJOBFILE.c_str(), std::ios::out | std::ios::binary);  // reset
    end_cjob();
  }
}

void Simulation::reset_counters() {
  ISET = -1;
  IMCSE = -1;
  IMCSD = -1;
  IMCS = -1;
  ICYC = -1;

  ISETstart = 0;
  IMCSDstart = 0;
  IMCSstart = 0;
}

void Simulation::set_NCYC() {
  AutoDebugDump("Simulation::set_NCYC");
  double vol = P.BETA * LAT.NSITE;

  double path;
  int ncyc = 1;
  int NSAMP = P.NPRE / 10;
  std::vector<int> ncycSAMP(NSAMP);

  for (IMCSE = 0; IMCSE < P.NPRE; IMCSE++) {
    path = 0.0;
    ncyc = 0;
    // To recover ergodicity of S=1/2 antiferromagnetic Heisenberg model
    while (path < vol || (LAT.NSITE == 2 && ncyc % 2 == 0)) {
      double p = Cycle(false);
      path += p;
      ncyc++;
    }

    ncycSAMP[IMCSE % NSAMP] = ncyc;
  }

  int ncycSUM = 0;
  for (int isamp = 0; isamp < NSAMP; isamp++) {
    ncycSUM += ncycSAMP[isamp];
  }

  P.NCYC = ncycSUM / NSAMP;
}

void Simulation::Set(int ntherm, int nmcs) {
  AutoDebugDump("Simulation::Set");

  ICYC = -1;

  for (IMCSD = IMCSDstart; IMCSD < ntherm; IMCSD++) {
    if (P.SIMTIME > 0) {
      if (cjob_timer.elapsed() > P.SIMTIME) {
        IMCS = 0;
        save();
        end_job();
      }
      isChainjob = false;
    }
    Sweep(false);
  }  // for (IMCSD)
  IMCSDstart = 0;
  if (!isChainjob) {
    MSR.setinit();
    calctimer.setinit();
    sf.setinit();
    cf.setinit();
    ck.setinit();
  } else {
    isChainjob = false;
  }

  for (IMCS = IMCSstart; IMCS < nmcs; IMCS++) {
    if (P.SIMTIME > 0.0) {
      if (cjob_timer.elapsed() > P.SIMTIME) {
        save();
        end_job();
      }
    }
    calctimer.reset_timer();
    Sweep(true);
    calctimer.measure();

    double sgn = calculate_sign();

    MSR.measure(sgn);
    MSR.accumulate_length(AMP, sgn);
    sf.measure(sgn);
    cf.accumulate(P.NCYC, sgn);
    ck.accumulate(P.NCYC, sgn);
  }
  IMCSstart = 0;

  MSR.setsummary();
  calctimer.setsummary();
  sf.setsummary();
  cf.setsummary();
  ck.setsummary();
}

void Simulation::Sweep(bool thermalized) {
  AutoDebugDump("Simulation::Sweep");
  if (thermalized) {
    cf.reset();
    ck.reset();
  }
  double len = 0.0;
  for (ICYC = 0; ICYC < P.NCYC; ICYC++) {
    len += Cycle(thermalized);
  }
  AMP = len / P.NCYC;
}

double Simulation::Cycle(bool thermalized) {
  AutoDebugDump("Simulation::Cycle");
  if (!PlaceWorm()) return 0.0;
  return MoveHead(thermalized);
}

/*
 * Try to place a pair of wormheads uniform randomly
 * Return true  if wormheads are inserted
 *        false if not
 */
bool Simulation::PlaceWorm() {
  AutoDebugDump("Simulation::PlaceWorm");

  Vertex& Vorg = W.origin();
  W.setCurrentVertex(Vorg);

  // timespace where worm will be inserted
  int s = RND.Int(LAT.NSITE);
  Site& ST = LAT.S(s);
  double t = ST.getBeta() * RND.Uniform();

  SiteProperty& SP = ST.Property();
  VertexProperty& VP = SP.getVertexProperty();
  Vorg.BareVertex::init(t, VP);

  Vorg.setTime(t);
  Segment& S0 = ST.findS(t);

  int x = S0.X();

  SiteInitialConfiguration& IC = SP.getInitialConfiguration(x);

  int NCH = IC.NCH;
  int c;
  double r = RND.Uniform();
  for (c = 0; c < NCH; c++) {
    if (r < IC.CH[c].PROB) break;
  }
  ScatteringChannel& CH = IC.CH[c];
  int out = CH.OUT;
  int xout = CH.XOUT;

  // fail to insert
  if (out == DIR::UNDEF) {
    return false;
  }

  Segment& S1 = S0.cut(Vorg, 0);

  if (out == UORD::DOWN) {
    W.setCurrentSegment(S0);
  } else {
    W.setCurrentSegment(S1);
  }

  W.setXBEHIND(xout);

  tail_tau = t;
  tail_site = s;

  return true;
}

double Simulation::MoveHead(bool thermalized) {
  AutoDebugDump("Simulation::MoveHead");
  double len = 0.0;
  double len0 = 0.0;
  double len1 = 0.0;
  EndOfCycle = false;

  while (true) {
    if (W.getUORD()) {
      len0 = DOWN_ONESTEP(thermalized);
      len += len0;
      len1 -= len0;
    } else {
      len0 = UP_ONESTEP(thermalized);
      len += len0;
      len1 += len0;
    }
    if (EndOfCycle) break;
  }

#ifdef DEB
  if (!W.atOrigin()) {
    fprintf(FERR, "\nMoveHead> ### Error.\n");
    fprintf(FERR, " ... Hasn't come back to the origin.\n");
    // dump();
    exit(0);
  }
#endif

  W.remove();
  return len;
}

double Simulation::UP_ONESTEP(bool thermalized) {
  AutoDebugDump("Simulation::UP_ONESTEP");
  DebugDumpNV(thermalized);
  double len = 0.0;
  double RHO = 0.0;

  Segment& c_S = W.getCurrentSegment();
  Site& c_Site = (*c_S.getONSITE());

  Interaction** CI = c_Site.getCI();
  const int NCI = c_Site.getNCI();  // the number of Interaction:

  Vertex& c_V = W.getCurrentVertex();
  Vertex& n_V = W.getNextVertex();

  const int xinc = W.getXBEHIND();

  // Using a std::vector is slightly slower (1~5%) than using a raw pointer
  // for some environment... X(
  UniformInterval* UI = new UniformInterval[NCI];

  Ring<RegVInfo> RingRVI;
  RingRVI.ROOT.V_x = &V4REF;

  const double n_Vtime = n_V.isTerminal() ? LAT.BETA : n_V.time();
  const double c_time = c_V.isTerminal() ? 0.0 : c_V.time();
  const double n_time = n_Vtime - c_time;
  if (c_V.isTerminal()) {
    for (int iCI = 0; iCI < NCI; iCI++) {
      UniformInterval& ui = UI[iCI];
      ui.init(CI[iCI], xinc);
      for (int i_body = 0; i_body < ui.nbody; i_body++) {
        if (&(CI[iCI]->site(i_body)) == &c_Site) {
          ui.inc = 2 * i_body;
          ui.n_S[i_body] = &c_S;
        } else {
          ui.n_S[i_body] = &(CI[iCI]->site(i_body).head());

          Vertex& near_V = ui.n_S[i_body]->top();
          const double V_time = near_V.time();
          if ((!near_V.isTerminal()) && (V_time < n_time) &&
              (&near_V != &n_V)) {
            RegVInfo& RVI = TheRVIPool.pop();
            RVI.setRVI(&near_V, iCI, i_body, V_time);
            RingRVI.add_tail(RVI);
          }
        }
      }
      ui.setx();
      ui.setVIC();

      if (ui.DefinedVIC) {
        RHO += ui.VIC->dRHO;
      }
    }
  } else {
    for (int iCI = 0; iCI < NCI; iCI++) {
      UniformInterval& ui = UI[iCI];
      ui.init(CI[iCI], xinc);
      for (int i_body = 0; i_body < ui.nbody; i_body++) {
        if (&(CI[iCI]->site(i_body)) == &c_Site) {
          ui.inc = 2 * i_body;
          ui.n_S[i_body] = &c_S;
        } else {
          if (CI[iCI] == c_V.getONINTERACTION()) {
            ui.n_S[i_body] = &(c_V.S(2 * i_body + 1));
          } else {
            ui.n_S[i_body] = &(CI[iCI]->site(i_body).findS(c_time));
          }
          Vertex& near_V = ui.n_S[i_body]->top();
          const double V_time = near_V.time() - c_time;
          if ((!near_V.isTerminal()) && (V_time < n_time) &&
              (&near_V != &n_V)) {
            RegVInfo& RVI = TheRVIPool.pop();
            RVI.setRVI(&near_V, iCI, i_body, V_time);
            RingRVI.add_tail(RVI);
          }
        }
      }
      ui.setx();
      ui.setVIC();

      if (ui.DefinedVIC) {
        RHO += ui.VIC->dRHO;
      }
    }
  }
  double near_tau = 0.0;
  double try_tau = 0.0;
  int out, xout;

  while (true) {
    Ring<RegVInfo>::iterator it = RingRVI.sort_min();

    near_tau = (RingRVI.empty()) ? n_time - len : (*it).V_time - len;

    if (RHO > EPS) {
      try_tau = RND.Exp() / RHO;
      if (try_tau < near_tau) {
        len += try_tau;
        double RHORND = RHO * RND.Uniform();

        int i_UI = 0;
        int s_UI = 0;
        double iRHO = 0.0;
        double sRHO = 0.0;
        do {
          sRHO = iRHO;
          if (UI[i_UI].DefinedVIC) {
            iRHO += UI[i_UI].VIC->dRHO;
            s_UI = i_UI;
          }
          ++i_UI;
        } while (RHORND >= iRHO);
        RHORND -= sRHO;

        UniformInterval& ui = UI[s_UI];

        ScatteringChannel& CH = (*ui.VIC).getScatteringChannel(RHORND);

        const double setVtime = c_time + len;
        Vertex& setV = TheVertexPool.pop();
        setV.BareVertex::init(ui.I_n, setVtime, (*ui.VP));
        for (int ibody = 0; ibody < ui.nbody; ++ibody) {
          ui.n_S[ibody]->cut(setV, ibody);
        }
        ui.I_n->add_tail(setV);

        setV.S(ui.inc).setX(xinc);
        if (!(c_V.isTerminal() || c_V.isKink())) {
          c_V.erase();
        }
        W.setCurrentVertex(setV);
        W.setCurrentSegment(setV.S(CH.OUT));
        W.setXBEHIND(CH.XOUT);

        break;
      }
    }
    if (RingRVI.empty()) {
      W.setCurrentVertex(n_V);
      const int inc = n_V.which(c_S);
      if (n_V.isTerminal()) {
        out = 1 - inc;
        xout = xinc;
      } else {
        VertexInitialConfiguration& IC = n_V.getInitialConfiguration(inc, xinc);
        ScatteringChannel& CH = IC.getScatteringChannel();
        out = CH.OUT;
        xout = CH.XOUT;
      }

      c_S.setX(xinc);
      if (!(c_V.isTerminal() || c_V.isKink())) {
        c_V.erase();
      }
      if (out == DIR::UNDEF) {
        EndOfCycle = true;
      } else {
        W.setCurrentSegment(n_V.S(out));
        W.setXBEHIND(xout);
      }
      len += near_tau;
      break;
    }

    // UPDATE RegVI and UI
    Vertex* changeV = it->V_x;
    do {
      RegVInfo& RVImin = *it;
      UniformInterval& ui = UI[RVImin.i_UI];

      if (ui.DefinedVIC) {
        RHO -= ui.VIC->dRHO;
      }
      ui.n_S[RVImin.i_body] = &(ui.n_S[RVImin.i_body]->next());
      ui.setx(RVImin.i_body);
      ui.setVIC();
      if (ui.DefinedVIC) {
        RHO += ui.VIC->dRHO;
      }

      // add RegVI
      Vertex& near_V = (*ui.n_S[RVImin.i_body]).top();
      const double V_time = near_V.time() - c_time;
      if ((!near_V.isTerminal()) && (V_time < n_time) && (&near_V != &n_V)) {
        RegVInfo& RVI = TheRVIPool.pop();
        RVI.setRVI(&near_V, RVImin.i_UI, RVImin.i_body, V_time);
        RingRVI.add_tail(RVI);
      }
      ++it;
      RVImin.erase();
    } while (changeV == (*it).V_x);

    it = RingRVI.head();
    len += near_tau;
  }

  while (!RingRVI.empty()) {
    RegVInfo& DRVI = RingRVI.first();
    DRVI.remove();
    TheRVIPool.push(DRVI);
  }

  if (thermalized) {
    const int head_site = c_Site.id() - 1;
    cf.count(c_time + len, c_time, head_site, tail_site, tail_tau);
    ck.count((head_site - tail_site + LAT.NSITE) % LAT.NSITE, c_time + len,
             c_time, tail_tau);
  }

  delete[] UI;
  return len;
}

double Simulation::DOWN_ONESTEP(bool thermalized) {
  AutoDebugDump("Simulation::DOWN_ONESTEP");
  DebugDumpNV(thermalized);
  double len = 0.0;
  double RHO = 0.0;

  Segment& c_S = W.getCurrentSegment();
  Site& c_Site = (*c_S.getONSITE());

  Interaction** CI =
      c_Site.getCI();  //  Interaction* CI [n_int]; which belong to curSite
  const int NCI = c_Site.getNCI();  // the number of Interaction:

  Vertex& c_V = W.getCurrentVertex();
  Vertex& n_V = W.getNextVertex();

  const int xinc = W.getXBEHIND();

  // Using a std::vector is slightly slower (1~5%) than using a raw pointer
  // for some environment... X(
  UniformInterval* UI = new UniformInterval[NCI];

  Ring<RegVInfo> RingRVI;
  RingRVI.ROOT.V_x = &V4REF;
  const double n_Vtime = (n_V.isTerminal()) ? 0.0 : n_V.time();
  const double c_time = c_V.isTerminal() ? LAT.BETA : c_V.time();
  const double n_time = c_time - n_Vtime;

  if (c_V.isTerminal()) {
    for (int iCI = 0; iCI < NCI; iCI++) {
      UniformInterval& ui = UI[iCI];

      ui.init(CI[iCI], xinc);
      for (int i_body = 0; i_body < ui.nbody; i_body++) {
        if (&(CI[iCI]->site(i_body)) == &c_Site) {
          ui.inc = 2 * i_body + 1;
          ui.n_S[i_body] = &c_S;
        } else {
          ui.n_S[i_body] = &(CI[iCI]->site(i_body).tail());
          Vertex& near_V = ui.n_S[i_body]->bottom();
          const double V_time = c_time - near_V.time();
          if ((!near_V.isTerminal()) && (V_time < n_time) &&
              (&near_V != &n_V)) {
            RegVInfo& RVI = TheRVIPool.pop();
            RVI.setRVI(&near_V, iCI, i_body, V_time);
            RingRVI.add_tail(RVI);
          }
        }
      }
      ui.setx();
      ui.setVIC();
      if (ui.DefinedVIC) {
        RHO += (*(ui.VIC)).dRHO;
      }
    }

  } else {
    for (int iCI = 0; iCI < NCI; iCI++) {
      UniformInterval& ui = UI[iCI];

      ui.init(CI[iCI], xinc);
      for (int i_body = 0; i_body < ui.nbody; i_body++) {
        if (&(CI[iCI]->site(i_body)) == &c_Site) {
          ui.inc = 2 * i_body + 1;
          ui.n_S[i_body] = &c_S;
        } else {
          if (CI[iCI] == c_V.getONINTERACTION()) {
            ui.n_S[i_body] = &(c_V.S(2 * i_body));
          } else {
            ui.n_S[i_body] = &(CI[iCI]->site(i_body).findS(c_time));
          }
          Vertex& near_V = ui.n_S[i_body]->bottom();
          const double V_time = c_time - near_V.time();
          if ((!near_V.isTerminal()) && (V_time < n_time) &&
              (&near_V != &n_V)) {
            RegVInfo& RVI = TheRVIPool.pop();
            RVI.setRVI(&near_V, iCI, i_body, V_time);
            RingRVI.add_tail(RVI);
          }
        }
      }
      ui.setx();
      ui.setVIC();
      if (ui.DefinedVIC) {
        RHO += ui.VIC->dRHO;
      }
    }
  }
  double near_tau = 0.0;
  double try_tau = 0.0;
  int out, xout;

  while (true) {
    Ring<RegVInfo>::iterator it = RingRVI.sort_min();

    near_tau = RingRVI.empty() ? n_time - len : it->V_time - len;

    if (RHO > EPS) {
      try_tau = RND.Exp() / RHO;
      if (try_tau < near_tau) {
        len += try_tau;
        double RHORND = RHO * RND.Uniform();
        int i_UI = 0;
        int s_UI = 0;
        double iRHO = 0.0;
        double sRHO = 0.0;
        do {
          sRHO = iRHO;
          if (UI[i_UI].DefinedVIC) {
            iRHO += UI[i_UI].VIC->dRHO;
            s_UI = i_UI;
          }
          ++i_UI;
        } while (RHORND >= iRHO);
        RHORND -= sRHO;

        UniformInterval& ui = UI[s_UI];
        ScatteringChannel& CH = ui.VIC->getScatteringChannel(RHORND);

        double setVtime = c_time - len;
        Vertex& setV = TheVertexPool.pop();
        setV.BareVertex::init(ui.I_n, setVtime, *ui.VP);
        for (int ibody = 0; ibody < ui.nbody; ++ibody) {
          ui.n_S[ibody]->cut(setV, ibody);
        }
        ui.I_n->add_tail(setV);

        setV.S(ui.inc).setX(xinc);
        if (!(c_V.isTerminal() || c_V.isKink())) {
          c_V.erase();
        }
        W.setCurrentVertex(setV);
        W.setCurrentSegment(setV.S(CH.OUT));
        W.setXBEHIND(CH.XOUT);

        break;
      }
    }
    if (RingRVI.empty()) {
      W.setCurrentVertex(n_V);
      const int inc = n_V.which(c_S);
      if (n_V.isTerminal()) {
        out = 1 - inc;
        xout = xinc;
      } else {
        VertexInitialConfiguration& IC = n_V.getInitialConfiguration(inc, xinc);
        ScatteringChannel& CH = IC.getScatteringChannel();
        out = CH.OUT;
        xout = CH.XOUT;
      }

      c_S.setX(xinc);
      if (!(c_V.isTerminal() || c_V.isKink())) {
        c_V.erase();
      }
      if (out == DIR::UNDEF) {
        EndOfCycle = true;
      } else {
        W.setCurrentSegment(n_V.S(out));
        W.setXBEHIND(xout);
      }
      len += near_tau;
      break;
    }

    // UPDATE RegVI and UI
    Vertex* changeV = it->V_x;
    do {
      RegVInfo& RVImin = *it;
      UniformInterval& ui = UI[RVImin.i_UI];

      if (ui.DefinedVIC) {
        RHO -= ui.VIC->dRHO;
      }
      ui.n_S[it->i_body] = &(ui.n_S[(*it).i_body]->prev());  //
      ui.setx(it->i_body);
      ui.setVIC();
      if (ui.DefinedVIC) {
        RHO += ui.VIC->dRHO;
      }

      // add RegVI
      Vertex& near_V = ui.n_S[(*it).i_body]->bottom();
      const double V_time = c_time - near_V.time();
      if ((!near_V.isTerminal()) && (V_time < n_time) && (&near_V != &n_V)) {
        RegVInfo& RVI = TheRVIPool.pop();
        RVI.setRVI(&near_V, it->i_UI, it->i_body, V_time);
        RingRVI.add_tail(RVI);
      }
      ++it;
      RVImin.erase();
    } while (changeV == it->V_x);

    it = RingRVI.head();
    len += near_tau;
  }

  while (!RingRVI.empty()) {
    RegVInfo& DRVI = RingRVI.first();
    DRVI.remove();
    TheRVIPool.push(DRVI);
  }

  if (thermalized) {
    const int head_site = c_Site.id() - 1;
    cf.count(c_time, c_time - len, head_site, tail_site, tail_tau);
    ck.count((head_site - tail_site + LAT.NSITE) % LAT.NSITE, c_time,
             c_time - len, tail_tau);
  }

  delete[] UI;
  return len;
}

double Simulation::calculate_sign() {
  double sgn = 1.0;
  for (int b = 0; b < LAT.NINT; ++b) {
    Interaction& I = LAT.I(b);
    InteractionProperty& IP = I.property();
    int NLEG = 2 * IP.NBODY;
    std::vector<int> x(NLEG);

    Interaction::iterator itv(I.root());
    ++itv;
    for (; !itv.atOrigin(); ++itv) {
      Vertex& v = *itv;
      for (int l = 0; l < NLEG; ++l) {
        x[l] = v.X(l);
      }
      sgn *= IP.sign(x);
    }
  }
  return sgn;
}

inline void Simulation::dump(const char* s = "") {
  printf("\n>>> %s (iset=%d imcse=%d imcsd=%d imcs=%d icyc= %d)", s, ISET,
         IMCSE, IMCSD, IMCS, ICYC);
  LAT.dump();
  W.dump();
}

void Simulation::Check() {
#ifndef DEB
  return;
#endif

  bool ERROR = false;

  for (int b = 0; b < LAT.NINT; b++) {
    Interaction& I = LAT.I(b);
    if (I.empty()) continue;
    Interaction::iterator p(I);
    while (!(++p).atOrigin()) {
      ERROR = ERROR || p->check();
    }
  }

  for (int s = 0; s < LAT.NSITE; s++) {
    Site& S = LAT.S(s);
    Site::iterator p(S);
    while (!(++p).atOrigin()) {
      ERROR = ERROR || p->check();
    }
  }

  if (ERROR) {
    printf("DETECT ERROR\n");
    dump();
    exit(0);
  }
}
