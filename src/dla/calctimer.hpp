#ifndef CALCTIMER_HPP
#define CALCTIMER_HPP

#include "../common/timer.hpp"
#include "accumulator.hpp"

class CalcTimer {
  int NMCS;
  Timer timer;
  Accumulator ACC;
  Accumulator PHY;

public:
  CalcTimer(int nmcs) : NMCS(nmcs) {PHY.reset("time");}
  void reset_timer() { timer.reset(); }
  void setinit() { ACC.reset("time"); }
  void measure() { ACC.accumulate(timer.elapsed() / NMCS); }
  void setsummary() {
    ACC.average();
    PHY.accumulate(ACC.mean());
  }
  void summary() { PHY.average(); }
#ifdef MULTI
  void allreduce(MPI_Comm comm){ PHY.allreduce(comm);}
#endif
  void show(FILE* F) { PHY.show(F,"R"); }
  void load(std::ifstream& ifs){
    ACC.load(ifs);
    PHY.load(ifs);
  }
  void save(std::ofstream& ofs) const{
    ACC.save(ofs);
    PHY.save(ofs);
  }
};

#endif  // TIMER_HPP
