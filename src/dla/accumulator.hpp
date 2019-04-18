#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

//######################################################################

#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iomanip>

#ifdef MULTI
#include "mpi.h"
#endif

#include "name.h"
#include "serialize.hpp"

class Accumulator{
private:
  std::string k;
  double value;
  double error;
  double s1, s2;
  int n;


public:
  Accumulator() : k("Unset"){};
  explicit Accumulator(std::string const& s) : k(s){};

  void reset() {
    k = "Unset";
    value = 0.0;
    error = 0.0;
    s1 = 0.0;
    s2 = 0.0;
    n  = 0;
  };
  void reset(std::string const& s) {
    reset();
    set_key(s);
  };
  void set_key(std::string const& s) { k = s; };

  void accumulate(double x) {
    n++;
    s1 += x;
    s2 += (x * x);
  };

  void average() {
    value = 0.0;
    error = 0.0;
    if (n > 0) {
      value = s1 / n;
      error = s2 / n;
      if (n > 1) {
        error = error - value * value;
        error /= n - 1;
        error = sqrt(error);
      } else {
        error = 0.0;
      }
    }
  };

  const char* ckey() const { return k.c_str(); }
  std::string key() const { return k; }
  double mean() const { return value; }
  double std_error() const { return error;}

  std::string dump() const {
    std::stringstream ss;
    ss << k << " = " << std::setprecision(8) << std::scientific << value << " " << error;
    return ss.str();
  }
  void show() const { std::cout << dump() << std::endl;}
  void show(FILE* F) const {
    std::string s = dump();
    std::fprintf(F, "%s\n", s.c_str());
  }
  void show(const char* prefix) const {
    std::cout << prefix << " " << dump() << std::endl;
  }
  void show(FILE* F, const char* prefix) const {
    std::string s = dump();
    std::fprintf(F, "%s %s\n", prefix, s.c_str());
  }

  std::ofstream& save(std::ofstream& F) const{
    Serialize::save(F, k);
    Serialize::save(F, value);
    Serialize::save(F, error);
    Serialize::save(F, s1);
    Serialize::save(F, s2);
    Serialize::save(F, n);
    return F;
  }
  std::ifstream& load(std::ifstream& F){
    Serialize::load(F, k);
    Serialize::load(F, value);
    Serialize::load(F, error);
    Serialize::load(F, s1);
    Serialize::load(F, s2);
    Serialize::load(F, n);
    return F;
  }

#ifdef MULTI
  void allreduce(MPI_Comm comm){
    double s1_tmp, s2_tmp;
    int n_tmp;
    MPI_Allreduce(&s1, &s1_tmp, 1, MPI_DOUBLE, MPI_SUM, comm);
    MPI_Allreduce(&s2, &s2_tmp, 1, MPI_DOUBLE, MPI_SUM, comm);
    MPI_Allreduce(&n, &n_tmp, 1, MPI_INT, MPI_SUM, comm);
    s1 = s1_tmp;
    s2 = s2_tmp;
    n = n_tmp;
  }
#endif
};

namespace Serialize{
template <>
void save(std::ofstream& F, Accumulator const& acc){
  acc.save(F);
}
template <>
void load(std::ifstream& F, Accumulator & acc){
  acc.load(F);
}
}

#endif // ACCUMULATOR_H
