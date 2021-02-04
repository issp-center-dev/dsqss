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

#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/config.hpp>

#ifdef BOOST_NO_CXX11_HDR_CHRONO
#define BOOST_CHRONO_HEADER_ONLY
#include <boost/chrono.hpp>
namespace CHRONO = boost::chrono;
#else
#include <chrono>
namespace CHRONO = std::chrono;
#endif

class Timer {
  CHRONO::high_resolution_clock::time_point start;

 public:
  Timer() : start(CHRONO::high_resolution_clock::now()) {}
  void reset() { start = CHRONO::high_resolution_clock::now(); }
  double elapsed() const {
    CHRONO::high_resolution_clock::time_point end =
        CHRONO::high_resolution_clock::now();
    return CHRONO::duration_cast<CHRONO::microseconds>(end - start).count() *
           1.0e-6;
  }
};
#endif  // TIMER_HPP
