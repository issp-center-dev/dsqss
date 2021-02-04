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

#ifndef SRC_DLA_CALCTIMER_HPP_
#define SRC_DLA_CALCTIMER_HPP_

#include "../common/timer.hpp"
#include "accumulator.hpp"

class CalcTimer {
  int NMCS;
  Timer timer;
  Accumulator ACC;
  Accumulator PHY;

 public:
  explicit CalcTimer(int nmcs) : NMCS(nmcs) { PHY.reset("time"); }
  void reset_timer() { timer.reset(); }
  void setinit() { ACC.reset("time"); }
  void measure() { ACC.accumulate(timer.elapsed() / NMCS); }
  void setsummary() {
    ACC.average();
    PHY.accumulate(ACC.mean());
  }
  void summary() { PHY.average(); }
#ifdef MULTI
  void allreduce(MPI_Comm comm) { PHY.allreduce(comm); }
#endif
  void show(FILE* F) { PHY.show(F, "R"); }
  void load(std::ifstream& ifs) {
    ACC.load(ifs);
    PHY.load(ifs);
  }
  void save(std::ofstream& ofs) const {
    ACC.save(ofs);
    PHY.save(ofs);
  }
};

#endif  // SRC_DLA_CALCTIMER_HPP_
