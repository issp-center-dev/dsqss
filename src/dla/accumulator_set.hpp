#ifndef ACCUMULATOR_SET_H
#define ACCUMULATOR_SET_H

#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include "debug.hpp"
#include "accumulator.hpp"
#include "measure.hpp"

class Accumulator_set {
private:
  int ntau;
  int nelements;

  Accumulator SIGN;
  std::vector<std::vector<Accumulator> > ACC; // ACC[elem][tau]
  std::vector<std::vector<Accumulator> > PHY; // PHY[elem][tau]
  std::vector<std::vector<std::vector<Accumulator> > > ACC_COV; // ACC_COV[elem][tau][tau2]
  std::vector<std::vector<std::vector<Accumulator> > > PHY_COV; // PHY_COV[elem][tau][tau2]

public:
  Accumulator_set(int ntau, int nelements);

  int numtau() const { return ntau;}
  int numelements() const { return nelements;}

  void setinit();

  void accumulate_sign(double sgn) { SIGN.accumulate(sgn);}
  void accumulate(double sgn, int ielem, int itau, double val) {
    ACC[ielem][itau].accumulate(sgn*val);
  }
  void accumulate(double sgn, int ielem, int itau, int jtau, double ival, double jval) {
    ACC_COV[ielem][itau][jtau].accumulate(sgn*ival*jval);
  }
  void accumulate(double sgn, int ielem, std::vector<double> const & vals) {
    for(int itau=0; itau<ntau; ++itau){
      accumulate(sgn, ielem, itau, vals[itau]);
      for(int jtau=0; jtau<ntau; ++jtau){
        accumulate(sgn, ielem, itau, jtau, vals[itau], vals[jtau]);
      }
    }
  }
  void setsummary();
  void summary();
#ifdef MULTI
  void allreduce(MPI_Comm comm);
#endif
  void show(FILE*);

  void save(std::ofstream& F) const;
  void load(std::ifstream& F);

};

Accumulator_set::Accumulator_set(int ntau_, int nelements_)
  :ntau(ntau_), nelements(nelements_)
{
  AutoDebugDump("Accumulator_set::Accumulator_set");

  SIGN.reset();
  for(int ielem=0; ielem < nelements; ++ielem){
    ACC.push_back(std::vector<Accumulator>(ntau));
    PHY.push_back(std::vector<Accumulator>(ntau));
    ACC_COV.push_back(std::vector<std::vector<Accumulator> >(ntau, std::vector<Accumulator>(ntau)));
    PHY_COV.push_back(std::vector<std::vector<Accumulator> >(ntau, std::vector<Accumulator>(ntau)));
    for(int tau1=0; tau1<ntau; ++tau1){
      DebugDumpNV(tau1);
      std::stringstream ss;
      ss << "C" << ielem << "t" << tau1;
      ACC[ielem][tau1].reset();
      PHY[ielem][tau1].reset(ss.str());
      for(int tau2=0; tau2<ntau; ++tau2){
        std::stringstream ss2;
        ss2 << "C" << ielem << "t" << tau1 << "t" << tau2;
        ACC_COV[ielem][tau1][tau2].reset();
        PHY_COV[ielem][tau1][tau2].reset(ss2.str());
      }
    }
  }
}

inline void Accumulator_set::setinit() {
  AutoDebugDump("Accumulator_set::setinit");
  SIGN.reset();
  for (int i = 0; i < nelements; i++){
    for (int itau = 0; itau < ntau; ++itau){
      ACC[i][itau].reset();
      for (int itau2 = 0; itau2 < ntau; ++itau2){
        ACC_COV[i][itau][itau2].reset();
      }
    }
  }
}



inline void Accumulator_set::show(FILE* F) {
  AutoDebugDump("Accumulator_set::show");
  for (int i = 0; i < nelements; i++) {
    for (int it = 0; it < ntau; it++) {
      PHY[i][it].show(F,"R");
    }
    fprintf(F, "\n");
  }

  for (int i = 0; i < nelements; i++) {
    for (int it = 0; it < ntau; it++) {
      for (int jt = 0; jt < ntau; jt++) {
        PHY_COV[i][it][jt].show(F,"R");
      }
    }
    fprintf(F, "\n");
  }
};

inline void Accumulator_set::summary() {
  AutoDebugDump("Accumulator_set::summary");
  for (int i = 0; i < nelements; i++){
    for (int itau = 0; itau < ntau; itau++){
      PHY[i][itau].average();
      for (int jtau=0; jtau<ntau; ++jtau){
        PHY_COV[i][itau][jtau].average();
      }
    }
  }
}

void Accumulator_set::setsummary() {
  AutoDebugDump("Accumulator_set::setsummary");
  SIGN.average();
  const double invsign = 1.0/SIGN.mean();
  std::vector<double> gt(ntau);
  for (int ielem = 0; ielem < nelements; ielem++) {
    for (int it = 0; it < ntau; it++) {
      ACC[ielem][it].average();
      gt[it] = invsign*ACC[ielem][it].mean();
      PHY[ielem][it].accumulate(gt[it]);
    }
    for (int it=0; it<ntau; ++it){
      for (int jt=0; jt<ntau; ++jt){
        ACC_COV[ielem][it][jt].average();
        double v = invsign * ACC_COV[ielem][it][jt].mean();
        v -= gt[it]*gt[jt];
        PHY_COV[ielem][it][jt].accumulate(v);
      }
    }
  }
}

#ifdef MULTI
void Accumulator_set::allreduce(MPI_Comm comm){
  for (int ielem = 0; ielem < nelements; ielem++) {
    for (int it = 0; it < ntau; it++) {
      PHY[ielem][it].allreduce(comm);
      for (int jt = 0; jt < ntau; jt++){
        PHY_COV[ielem][it][jt].allreduce(comm);
      }
    }
  }
}
#endif

void Accumulator_set::save(std::ofstream& F) const {
  Serialize::save(F, SIGN);
  Serialize::save(F, ACC);
  Serialize::save(F, PHY);
  Serialize::save(F, ACC_COV);
  Serialize::save(F, PHY_COV);
}

void Accumulator_set::load(std::ifstream& F) {
  Serialize::load(F, SIGN);
  Serialize::load(F, ACC);
  const int nsf = ACC.size();
  if(nsf != nelements){
    std::cerr << "ERROR: nelements is mismatched" << std::endl;
    std::cerr << "       sfinpfile maybe changed." << std::endl;
    exit(1);
  }
  if(nsf > 0){
    const int nt = ACC[0].size();
    if(nt != ntau){
      std::cerr << "ERROR: ntau is mismatched" << std::endl;
      std::cerr << "       sfinpfile maybe changed." << std::endl;
      exit(1);
    }
  }
  Serialize::load(F, PHY);
  Serialize::load(F, ACC_COV);
  Serialize::load(F, PHY_COV);
}

#endif // ACCUMULATOR_SET_H
